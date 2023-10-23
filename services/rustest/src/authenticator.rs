#![forbid(unsafe_code)]
use std::sync::Arc;

use aide::OperationIo;
use anyhow::Context;
use axum::response::IntoResponse;
use hmac::{Hmac, Mac};
use hyper::StatusCode;
use jwt::{SignWithKey, VerifyWithKey};
use schemars::JsonSchema;
use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256, Sha384};
use thiserror::Error;

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
}

impl Authenticator {
    pub fn new(storage: Arc<ETCDRustestStorage>) -> Self {
        Self { storage }
    }

    pub async fn authenticate(
        &self,
        req: UserLoginRequest,
    ) -> Result<UserLoginResponse, AuthenticationError> {
        let user = self
            .storage
            .get_user(&req.login)
            .await
            .context("cannot authenticate user")?;
        let user = match user {
            Some(user) => user,
            None => return Err(AuthenticationError::UserNotFound { login: req.login }),
        };

        let password_hash = Self::hash_password(&req.password);
        if user.hash_password != password_hash {
            return Err(AuthenticationError::InvalidPassword { login: req.login });
        }

        Ok(UserLoginResponse {
            token: Self::generate_token_for_user(&user),
        })
    }

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

        let user = User {
            login: req.login,
            hash_password: Self::hash_password(&req.password),
            bio: req.bio,
        };

        self.storage
            .create_user(user.clone())
            .await
            .context("cannot register user")?;

        Ok(RegisterUserResponse {
            token: Self::generate_token_for_user(&user),
        })
    }

    pub async fn validate_token(&self, token: &str) -> Result<String, AuthenticationError> {
        let key: Hmac<Sha384> =
            Hmac::new_from_slice(b"some-secret").expect("hmac can use key of any size");
        let user_token: UserToken = token
            .verify_with_key(&key)
            .map_err(|_| AuthenticationError::InvalidToken)?;

        Ok(user_token.login)
    }

    fn generate_token_for_user(user: &User) -> String {
        let key: Hmac<Sha384> =
            Hmac::new_from_slice(b"some-secret").expect("hmac can use key of any size");
        let user_token = UserToken {
            login: user.login.clone(),
        };

        user_token.sign_with_key(&key).expect("jwt always succeeds")
    }

    fn hash_password(password: &str) -> String {
        let salt = "secret_penis_salt228";
        format!("{:x}", Sha256::digest(password.to_string() + salt))
    }
}
