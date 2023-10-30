#![forbid(unsafe_code)]

use anyhow::{bail, Context, Result};
use schemars::JsonSchema;
use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Serialize, Deserialize, JsonSchema)]
pub struct Question {
    pub question: String,
    pub allowed_answers: Vec<String>,
    pub correct_idx: u32,
}

impl Question {
    pub fn validate(&self) -> Result<()> {
        if self.correct_idx as usize >= self.allowed_answers.len() {
            bail!(
                "correct idx of question must be in bounds 0 <= correct_idx < len(allowed_answers). but correct_idx = {} and len(allowed_answers) = {}",
                self.correct_idx,
                self.allowed_answers.len(),
            );
        }

        Ok(())
    }
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

impl Rustest {
    pub fn validate(&self) -> Result<()> {
        for (idx, question) in self.questions.iter().enumerate() {
            question.validate().context(format!(
                "invalid rustest provided: `{}'s` question wasn't valid",
                idx
            ))?;
        }

        Ok(())
    }
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
