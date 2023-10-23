#![forbid(unsafe_code)]

use schemars::JsonSchema;
use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Serialize, Deserialize, JsonSchema)]
pub struct Question {
    pub question: String,
    pub allowed_answers: Vec<String>,
    pub correct_idx: u32,
}

#[derive(Debug, Clone, Serialize, Deserialize, JsonSchema)]
pub struct Rustest {
    #[serde(default)]
    pub id: String,

    pub name: String,
    pub description: String,
    pub reward: String,
    pub questions: Vec<Question>,

    #[serde(default)]
    pub owner: String,
}

#[derive(Debug, Clone, Serialize, Deserialize, JsonSchema)]
pub struct User {
    pub login: String,
    pub hash_password: String,

    #[serde(default)]
    pub bio: String,
}

#[derive(Debug, Clone, Serialize, Deserialize, JsonSchema)]
pub struct RustestUserState {
    pub points: u32,
    pub cur_round: u32,
}
