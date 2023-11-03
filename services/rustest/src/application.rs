#![forbid(unsafe_code)]
use std::sync::Arc;

use aide::OperationIo;
use anyhow::{Context, Result};
use axum::response::IntoResponse;
use hyper::StatusCode;

use schemars::JsonSchema;
use serde::{Deserialize, Serialize};
use thiserror::Error;

use crate::{
    dto::{Question, Rustest, User},
    storage::ETCDRustestStorage, constants::PAGE_SIZE,
};

#[derive(Clone, Debug, Error, OperationIo)]
#[aide(output)]
pub enum RustestApplicationError {
    #[error("rustest application error: resource not found: {reason:}")]
    NotFound { reason: String },

    #[error("invalid data provided: {reason:}")]
    InvalidDataProvided { reason: String },

    #[error("action is unallowed: {action:}")]
    ActionNotAllowed { action: String },

    #[error("unknown error occurred: {err:}")]
    Unknown { err: String },
}

impl IntoResponse for RustestApplicationError {
    fn into_response(self) -> axum::response::Response {
        match &self {
            RustestApplicationError::NotFound { .. } => (StatusCode::NOT_FOUND, self.to_string()),
            RustestApplicationError::InvalidDataProvided { .. } => {
                (StatusCode::BAD_REQUEST, self.to_string())
            }
            RustestApplicationError::Unknown { .. } => {
                (StatusCode::INTERNAL_SERVER_ERROR, self.to_string())
            }
            RustestApplicationError::ActionNotAllowed { .. } => {
                (StatusCode::UNAUTHORIZED, self.to_string())
            }
        }
        .into_response()
    }
}

impl From<anyhow::Error> for RustestApplicationError {
    fn from(err: anyhow::Error) -> Self {
        RustestApplicationError::Unknown {
            err: err.to_string(),
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, Serialize, JsonSchema)]
pub struct QuestionWithoutAnswer {
    pub question: String,
    pub allowed_answers: Vec<String>,
}

impl From<Question> for QuestionWithoutAnswer {
    fn from(q: Question) -> Self {
        Self {
            question: q.question,
            allowed_answers: q.allowed_answers,
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, Serialize, JsonSchema)]
#[serde(tag = "result")]
pub enum FinalRustestState {
    Win { reward: String },
    Lose,
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
#[serde(tag = "state")]
pub enum RustestUserStateResponse {
    Running {
        question: QuestionWithoutAnswer,
        round: u32,
    },
    Final {
        final_state: FinalRustestState,
    },
}

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct RustestUserStateRequest {
    pub user_id: String,
    pub test_id: String,
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct SubmitAnswerRequest {
    #[serde(skip_deserializing)]
    pub user_id: String,

    #[serde(skip_deserializing)]
    pub test_id: String,

    pub round: u32,
    pub answer: u32,
}

pub type SubmitAnswerResponse = RustestUserStateResponse;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct CreateRustestRequest {
    #[serde(skip_deserializing)]
    pub user_id: String,

    #[serde(flatten)]
    pub rustest: Rustest,
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
pub struct CreateRustestResponse {
    #[serde(flatten)]
    pub rustest: Rustest,
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct UserRustestsRequest {
    pub user_id: String,
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
pub struct UserRustestsResponse {
    pub rustests: Vec<MaskedRustest>,
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct GetRustestRequest {
    pub test_id: String,
    pub user_id: String,
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
pub struct MaskedRustest {
    pub id: String,
    pub name: String,
    pub owner: String,
    pub description: String,
}

impl From<Rustest> for MaskedRustest {
    fn from(test: Rustest) -> Self {
        MaskedRustest {
            id: test.id,
            name: test.name,
            owner: test.owner,
            description: test.description,
        }
    }
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
#[serde(untagged)]
pub enum GetRustestResponse {
    Masked(MaskedRustest),
    Full(Rustest),
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct GetUsersRequest {
    #[serde(default)]
    page: usize,
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
pub struct MaskedUser {
    login: String,
    bio: String,
}

impl From<User> for MaskedUser {
    fn from(user: User) -> Self {
        Self {
            login: user.login,
            bio: user.bio,
        }
    }
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
pub struct GetUsersResponse {
    pages_total: usize,
    users: Vec<MaskedUser>,
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Debug, Deserialize, JsonSchema)]
pub struct GetRustestsRequest {
    #[serde(default)]
    page: usize,
}

#[derive(Clone, Debug, Serialize, JsonSchema)]
pub struct GetRustestsResponse {
    pages_total: usize,
    rustests: Vec<MaskedRustest>,
}

pub struct RussApplication {
    storage: Arc<ETCDRustestStorage>,
}

impl RussApplication {
    pub fn new(storage: Arc<ETCDRustestStorage>) -> RussApplication {
        RussApplication { storage }
    }

    #[tracing::instrument(skip(self))]
    pub async fn get_rustest(
        &self,
        req: GetRustestRequest,
    ) -> Result<GetRustestResponse, RustestApplicationError> {
        let rustest = self
            .storage
            .get_rustest(&req.test_id)
            .await
            .context("cannot get rustest")?;

        let rustest = match rustest {
            Some(rustest) => rustest,
            None => {
                return Err(RustestApplicationError::NotFound {
                    reason: "rustest `{}` not found".to_string(),
                })
            }
        };

        if rustest.owner == req.user_id {
            return Ok(GetRustestResponse::Full(rustest));
        }

        Ok(GetRustestResponse::Masked(rustest.into()))
    }

    #[tracing::instrument(skip(self))]
    pub async fn get_rustest_user_state(
        &self,
        req: RustestUserStateRequest,
    ) -> Result<RustestUserStateResponse, RustestApplicationError> {
        tracing::debug!("trying to find rustest");

        let rustest = self
            .storage
            .get_rustest(&req.test_id)
            .await
            .context(format!(
                "cannot get rustest state with id `{}` for user `{}`",
                req.test_id, req.user_id
            ))?;

        let rustest = match rustest {
            Some(rustest) => rustest,
            None => {
                return Err(RustestApplicationError::NotFound {
                    reason: format!("rustest with id `{}` not found", req.test_id),
                })
            }
        };

        tracing::debug!("rustest found in storage");

        if req.user_id == rustest.owner {
            return Err(RustestApplicationError::ActionNotAllowed {
                action: format!(
                    "user `{}` trying to solve it's own test `{}`, it is unallowed",
                    req.user_id, req.test_id
                ),
            });
        }

        tracing::debug!("rustest can be solved by user, user differs from author");

        if !self
            .storage
            .user_exists(&req.user_id)
            .await
            .context("cannot get rustest state")?
        {
            return Err(RustestApplicationError::NotFound {
                reason: format!("user with id `{}` not found", req.user_id),
            });
        }

        tracing::debug!("user exists in database");

        // user and test existence checked, now we can retrieve user state for this test
        let user_state = self
            .storage
            .get_user_state_on_test(&req.user_id, &req.test_id)
            .await
            .context(format!(
                "cannot get rustest state with id `{}` for user `{}`",
                req.test_id, req.user_id
            ))?;

        tracing::debug!("got user state from database");

        if user_state.cur_round as usize >= rustest.questions.len() {
            // all rounds are seen, time for finals
            let final_state = if user_state.cur_round == user_state.points {
                tracing::info!("user won the victorine, replying with reward");

                FinalRustestState::Win {
                    reward: rustest.reward,
                }
            } else {
                tracing::info!("user lose the victorine");

                FinalRustestState::Lose
            };

            return Ok(RustestUserStateResponse::Final { final_state });
        }

        let question: QuestionWithoutAnswer = rustest.questions[user_state.cur_round as usize]
            .clone()
            .into();

        tracing::info!("round is in running state, replying with next question");

        Ok(RustestUserStateResponse::Running {
            question,
            round: user_state.cur_round,
        })
    }

    #[tracing::instrument(skip(self))]
    pub async fn submit_answer(
        &self,
        req: SubmitAnswerRequest,
    ) -> Result<SubmitAnswerResponse, RustestApplicationError> {
        let rustest = self
            .storage
            .get_rustest(&req.test_id)
            .await
            .context(format!(
                "cannot submit answer on test `{}` for user `{}`",
                req.test_id, req.user_id
            ))?;

        let rustest = match rustest {
            Some(rustest) => rustest,
            None => {
                return Err(RustestApplicationError::NotFound {
                    reason: format!("rustest with id `{}` not found", req.test_id),
                })
            }
        };

        tracing::debug!("got rustest from storage");

        if req.user_id == rustest.owner {
            return Err(RustestApplicationError::ActionNotAllowed {
                action: format!(
                    "user `{}` trying to submit answer to his own test `{}`, it is unallowed",
                    req.user_id, req.test_id
                ),
            });
        }

        tracing::debug!("user is able to solve test");

        if !self
            .storage
            .user_exists(&req.user_id)
            .await
            .context("cannot submit answer")?
        {
            return Err(RustestApplicationError::NotFound {
                reason: format!("cannot find user with id `{}`", req.user_id),
            });
        }

        tracing::debug!("user exists");

        // user and rustest exist, we can retrieve their's state
        let user_state = self
            .storage
            .get_user_state_on_test(&req.user_id, &req.test_id)
            .await
            .context(format!(
                "cannot submit answer on test `{}` for user `{}`",
                req.test_id, req.user_id
            ))?;

        tracing::debug!("got user state on test");

        if user_state.cur_round != req.round {
            return Err(RustestApplicationError::InvalidDataProvided {
                reason: format!(
                    "user `{}` trying to send answer for round `{}`, but current round is `{}` in test `{}`",
                    req.user_id, req.round, user_state.cur_round, req.test_id,
                ),
            });
        }

        tracing::debug!("user trying to submit answer on correct round");

        if req.round as usize >= rustest.questions.len() {
            return Err(RustestApplicationError::InvalidDataProvided {
                reason: format!(
                    "user `{}` trying to send answer in test `{}` to already finished rustest",
                    req.user_id, req.test_id
                ),
            });
        }

        let question = rustest.questions.get(req.round as usize).unwrap();
        let increment_points = req.answer == question.correct_idx;
        tracing::info!(increment_points);

        let next_state = self
            .storage
            .increment_user_round_on_test(
                &req.user_id,
                &req.test_id,
                increment_points,
                Some(user_state),
            )
            .await
            .context(format!(
                "cannot submit answer on test `{}` for user `{}`",
                req.test_id, req.user_id
            ))?;

        tracing::debug!("state of user persisted to database");

        if next_state.cur_round as usize >= rustest.questions.len() {
            let final_state = if next_state.points == next_state.cur_round {
                tracing::info!("user won the victorine! replying with reward");

                FinalRustestState::Win {
                    reward: rustest.reward,
                }
            } else {
                tracing::info!("user lose the victorine");

                FinalRustestState::Lose
            };

            return Ok(SubmitAnswerResponse::Final { final_state });
        }

        tracing::info!("state is not final, replying with next question");

        Ok(SubmitAnswerResponse::Running {
            question: rustest.questions[next_state.cur_round as usize]
                .clone()
                .into(),
            round: next_state.cur_round,
        })
    }

    #[tracing::instrument(skip(self))]
    pub async fn create_rustest(
        &self,
        req: CreateRustestRequest,
    ) -> Result<CreateRustestResponse, RustestApplicationError> {
        if !self
            .storage
            .user_exists(&req.user_id)
            .await
            .context("cannot create rustest with author")?
        {
            return Err(RustestApplicationError::NotFound {
                reason: format!("user with id `{}` not found", req.user_id),
            });
        }

        tracing::debug!("user exists");

        req.rustest
            .validate()
            .context("cannot create rustest, rustest is invalid")
            .map_err(|err| RustestApplicationError::InvalidDataProvided {
                reason: err.to_string(),
            })?;
        
        tracing::info!("provided rustest is valid");

        let test = self
            .storage
            .create_rustest_with_author(req.rustest, &req.user_id)
            .await
            .context(format!("cannot create rustest for user `{}`", req.user_id))?;

        tracing::debug!("rustest persisted to database");

        Ok(CreateRustestResponse { rustest: test })
    }

    #[tracing::instrument(skip(self))]
    pub async fn get_users_rustests(
        &self,
        req: UserRustestsRequest,
    ) -> Result<UserRustestsResponse, RustestApplicationError> {
        if !self
            .storage
            .user_exists(&req.user_id)
            .await
            .context("cannot get rustests of user")?
        {
            return Err(RustestApplicationError::NotFound {
                reason: format!("user with id `{}` not found", req.user_id),
            });
        }

        tracing::debug!("user exists");

        let rustests = self
            .storage
            .rustests_of_user(&req.user_id)
            .await
            .context(format!("cannot fetch rustests of `{}` user", req.user_id))
            .map(|rustests| UserRustestsResponse {
                rustests: rustests.into_iter().map(|val| val.into()).collect(),
            })?;
        
        tracing::debug!("rustests of user retrieved successfully");

        Ok(rustests)
    }

    #[tracing::instrument(skip(self))]
    pub async fn get_all_users_by_page(
        &self,
        req: GetUsersRequest,
    ) -> Result<GetUsersResponse, RustestApplicationError> {
        let resp = self
            .storage
            .get_users_by_page(req.page, PAGE_SIZE)
            .await
            .context(format!("cannot get {}'s page of users", req.page))
            .map(|(users, cnt)| GetUsersResponse {
                pages_total: (cnt + PAGE_SIZE - 1) / PAGE_SIZE,
                users: users.into_iter().map(|user| user.into()).collect(),
            })?;

        tracing::debug!("successfuly retrieved page of users");

        Ok(resp)
    }

    pub async fn get_all_rustests_by_page(
        &self,
        req: GetRustestsRequest,
    ) -> Result<GetRustestsResponse, RustestApplicationError> {
        let resp = self
            .storage
            .get_rustests_by_page(req.page, PAGE_SIZE)
            .await
            .context(format!("cannot get {}'s page of rustests", req.page))
            .map(|(rustests, cnt)| GetRustestsResponse {
                pages_total: (cnt + PAGE_SIZE - 1) / PAGE_SIZE,
                rustests: rustests.into_iter().map(|rustest| rustest.into()).collect(),
            })?;
        
        tracing::debug!("successfully retrieved page of rustests");

        Ok(resp)
    }
}
