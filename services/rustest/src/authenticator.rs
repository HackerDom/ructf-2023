#![forbid(unsafe_code)]
use std::sync::Arc;

use aide::OperationIo;
use anyhow::Context;
use axum::response::IntoResponse;
use base64::Engine;
use hmac::{Hmac, Mac};
use hyper::StatusCode;
use jwt::{SignWithKey, VerifyWithKey};
use schemars::JsonSchema;
use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256, Sha384};
use thiserror::Error;
use tokio::{fs::File, io::AsyncReadExt};

use crate::{dto::User, storage::ETCDRustestStorage};

#[derive(Clone, Debug, Error, OperationIo)]
#[aide(output)]
pub enum AuthenticationError {
    #[error("user with login `{login:}` already exists")]
    UserAlreadyExists { login: String },

    #[error("invalid token provided")]
    InvalidToken,

    #[error("user with login `{login:}` not found, cannot authenticate")]
    UserNotFound { login: String },

    #[error("invalid password provided for user with login `{login:}`")]
    InvalidPassword { login: String },

    #[error("login `{login:}` is invalid: {reason:}")]
    InvalidLogin { login: String, reason: String },

    #[error("unexpected error during authentication occurred: {reason:}")]
    Unexpected { reason: String },
}

impl IntoResponse for AuthenticationError {
    fn into_response(self) -> axum::response::Response {
        match &self {
            AuthenticationError::UserAlreadyExists { .. } => {
                (StatusCode::BAD_REQUEST, self.to_string())
            }
            AuthenticationError::InvalidToken => (StatusCode::UNAUTHORIZED, self.to_string()),
            AuthenticationError::UserNotFound { .. } => (StatusCode::NOT_FOUND, self.to_string()),
            AuthenticationError::InvalidPassword { login } => {
                // hide invalid password error from client
                (
                    StatusCode::NOT_FOUND,
                    AuthenticationError::UserNotFound {
                        login: login.clone(),
                    }
                    .to_string(),
                )
            }
            AuthenticationError::Unexpected { .. } => {
                (StatusCode::INTERNAL_SERVER_ERROR, self.to_string())
            }
            AuthenticationError::InvalidLogin { .. } => (StatusCode::BAD_REQUEST, self.to_string()),
        }
        .into_response()
    }
}

impl From<anyhow::Error> for AuthenticationError {
    fn from(err: anyhow::Error) -> Self {
        AuthenticationError::Unexpected {
            reason: err.to_string(),
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub type Token = String;

#[derive(Clone, Debug, Serialize, Deserialize)]
struct UserToken {
    login: String,
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct RegisterUserRequest {
    pub login: String,
    pub password: String,

    #[serde(default)]
    pub bio: String,
}

impl RegisterUserRequest {
    pub fn validate(&self) -> Result<(), AuthenticationError> {
        validate_login(&self.login)
    }
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
pub struct RegisterUserResponse {
    pub token: String,
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct UserLoginRequest {
    pub login: String,
    pub password: String,
}

impl UserLoginRequest {
    pub fn validate(&self) -> Result<(), AuthenticationError> {
        validate_login(&self.login)
    }
}

pub type UserLoginResponse = RegisterUserResponse;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

fn validate_login(login: &str) -> Result<(), AuthenticationError> {
    if login.len() > 32 {
        return Err(AuthenticationError::InvalidLogin {
            login: login.to_string(),
            reason: "too long login, must be less than 32 symbols".to_string(),
        });
    }

    if !login.chars().all(|c| c.is_ascii_alphanumeric()) {
        return Err(AuthenticationError::InvalidLogin {
            login: login.to_string(),
            reason: "only latin letters and digits are permitted to use in login".to_string(),
        });
    }

    Ok(())
}

pub struct Authenticator {
    storage: Arc<ETCDRustestStorage>,
    salt: Option<String>,
    jwt_secret: Option<Hmac<Sha384>>,
}

impl Authenticator {
    pub fn new(storage: Arc<ETCDRustestStorage>) -> Self {
        Self {
            storage,
            salt: None,
            jwt_secret: None,
        }
    }

    pub async fn init(&mut self) -> Result<(), AuthenticationError> {
        let salt_bytes = read_urandom(32).await;
        let genered_salt = base64::prelude::BASE64_STANDARD_NO_PAD.encode(salt_bytes);
        let salt = self
            .storage
            .get_or_create_secret("salt", &genered_salt)
            .await
            .context("cannot init authenticator")?;

        let jwt_bytes = read_urandom(32).await;
        let genered_jwt_secret = base64::prelude::BASE64_STANDARD_NO_PAD.encode(jwt_bytes);
        let jwt_secret = self
            .storage
            .get_or_create_secret("jwt", &genered_jwt_secret)
            .await
            .context("cannot init authenticator")?;
        let jwt_hmac =
            Hmac::new_from_slice(jwt_secret.as_bytes()).expect("hmac can use key of any size");

        self.jwt_secret = Some(jwt_hmac);
        self.salt = Some(salt);

        Ok(())
    }

    #[tracing::instrument(skip(self))]
    pub async fn authenticate(
        &self,
        req: UserLoginRequest,
    ) -> Result<UserLoginResponse, AuthenticationError> {
        let user = self
            .storage
            .get_user(&req.login)
            .await
            .context("cannot authenticate user")?;

        tracing::debug!("got response about user from storage");

        let user = match user {
            Some(user) => user,
            None => return Err(AuthenticationError::UserNotFound { login: req.login }),
        };

        tracing::debug!("user exists");

        let password_hash = self.hash_password(&req.password);
        if user.hash_password != password_hash {
            return Err(AuthenticationError::InvalidPassword { login: req.login });
        }

        tracing::debug!("hashes of password are equal, user is now authenticated");

        Ok(UserLoginResponse {
            token: self.generate_token_for_user(&user),
        })
    }

    #[tracing::instrument(skip(self))]
    pub async fn register(
        &self,
        req: RegisterUserRequest,
    ) -> Result<RegisterUserResponse, AuthenticationError> {
        if self
            .storage
            .user_exists(&req.login)
            .await
            .context("cannot register user")?
        {
            return Err(AuthenticationError::UserAlreadyExists { login: req.login });
        }

        tracing::debug!("user is not present in database, could be registered");

        let user = User {
            login: req.login,
            hash_password: self.hash_password(&req.password),
            bio: req.bio,
        };

        self.storage
            .create_user(user.clone())
            .await
            .context("cannot register user")?;

        tracing::debug!("user persisted to database");

        Ok(RegisterUserResponse {
            token: self.generate_token_for_user(&user),
        })
    }

    #[tracing::instrument(skip(self))]
    pub async fn validate_token(&self, token: &str) -> Result<String, AuthenticationError> {
        let key = self
            .jwt_secret
            .as_ref()
            .expect("cannot validate token, authenticator is not initialized");

        let user_token: UserToken = token
            .verify_with_key(key)
            .map_err(|_| AuthenticationError::InvalidToken)?;

        Ok(user_token.login)
    }

    fn generate_token_for_user(&self, user: &User) -> String {
        let key = self
            .jwt_secret
            .as_ref()
            .expect("cannot generate token for user, authenticator is not initialized");

        let user_token = UserToken {
            login: user.login.clone(),
        };

        user_token.sign_with_key(key).expect("jwt always succeeds")
    }

    fn hash_password(&self, password: &str) -> String {
        let salt = self
            .salt
            .as_ref()
            .expect("cannot hash password, authenticator is not initialized");
        format!("{:x}", Sha256::digest(password.to_string() + salt))
    }
}

async fn read_urandom(amount: usize) -> Vec<u8> {
    let mut f = File::open("/dev/urandom").await.unwrap();
    let mut buf = vec![0u8; amount];
    f.read_exact(&mut buf).await.unwrap();

    buf
}
