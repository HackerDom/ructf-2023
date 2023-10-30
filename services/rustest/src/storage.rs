#![forbid(unsafe_code)]
use std::{collections::HashMap, num::NonZeroUsize};

use crate::dto::{Rustest, RustestUserState, User};
use anyhow::{anyhow, bail, Context, Result};
use etcd_rs::{
    Client, KeyRange, KeyValueOp, PutRequest, RangeRequest, RangeResponse, SortOrder, TxnCmp,
    TxnOpResponse, TxnRequest,
};
use lru::LruCache;
use serde::{de::DeserializeOwned, Serialize};
use thiserror::Error;
use tokio::sync::{Mutex, RwLock};

use uuid::Uuid;

#[derive(Debug, Clone, Error)]
pub enum RustestStorageError {
    #[error("key `{key:}` not found in storage")]
    NotFound { key: String },

    #[error("underlying storage error occurred: {reason:}")]
    UnderlyingStorage { reason: String },

    #[error("cannot deserialize value: {reason:}")]
    Deserialization { reason: String },

    #[error("cannot put key to etcd, key `{key:}` already exists")]
    AlreadyExists { key: String },

    #[error("unexpected error occurred in etcd storage: {reason:}")]
    Unexpected { reason: String },
}

pub struct ETCDRustestStorage {
    user_cache: Mutex<LruCache<String, User>>,
    rustest_cache: Mutex<LruCache<String, Rustest>>,
    users_offset_to_revision: RwLock<HashMap<u64, i64>>,
    rustests_offset_to_revision: RwLock<HashMap<u64, i64>>,
    client: Client,
}

impl ETCDRustestStorage {
    pub fn new(client: Client) -> ETCDRustestStorage {
        ETCDRustestStorage {
            client,
            user_cache: Mutex::new(LruCache::new(NonZeroUsize::new(10_000).unwrap())),
            rustest_cache: Mutex::new(LruCache::new(NonZeroUsize::new(10_000).unwrap())),
            users_offset_to_revision: Default::default(),
            rustests_offset_to_revision: Default::default(),
        }
    }

    /// Warms caches before storage starts working.
    pub async fn warm_caches(&self, _page_size: usize) -> Result<()> {
        Ok(())
    }

    /// Returns test from etcd storage. If test not found, None is returned.
    pub async fn get_rustest(&self, test_id: &str) -> Result<Option<Rustest>> {
        let mut cache_guard = self.rustest_cache.lock().await;
        if let Some(test) = cache_guard.get(test_id).cloned() {
            return Ok(Some(test));
        }

        let test: Option<Rustest> =
            get_deserialized_value_by_key(&self.client, format!("/rustest/rustests/{}", test_id))
                .await
                .map_err(|err| anyhow!(err))
                .context("cannot fetch rustest")?;

        if let Some(test) = test {
            cache_guard.put(test_id.to_string(), test.clone());
            return Ok(Some(test));
        }

        Ok(None)
    }

    /// Increments user round in persistent storage and returns resulting state.
    /// If increment points is true increments points also.
    /// Doesn't check user or rustest existence. Caller must do so.
    /// If user or rustest doesn't exist, creating needed keys anyway.
    /// If previous_state is not none, uses it as a hint for update. Allows to save one network call.
    /// If previous_state is None, makes request to etcd storage to retrieve state.
    pub async fn increment_user_round_on_test(
        &self,
        user_id: &str,
        test_id: &str,
        increment_points: bool,
        previous_state: Option<RustestUserState>,
    ) -> Result<RustestUserState> {
        let mut new_state = match previous_state {
            Some(state) => state,
            None => self
                .get_user_state_on_test(user_id, test_id)
                .await
                .context("cannot increment user round on test")?,
        };

        new_state.cur_round += 1;

        let round_key = format!("/rustest/user/{}/state/{}/cur_round", user_id, test_id);
        let mut txn_request = TxnRequest::default().and_then(
            gen_put_request_with_serialized_value(round_key, &new_state.cur_round)?,
        );

        if increment_points {
            let points_key = format!("/rustest/user/{}/state/{}/points", user_id, test_id);
            new_state.points += 1;
            txn_request = txn_request.and_then(gen_put_request_with_serialized_value(
                points_key,
                &new_state.points,
            )?);
        }

        self.client
            .txn(txn_request)
            .await
            .context("cannot increment user round")
            .map(|_| new_state)
    }

    /// Returns user state for provided user on given test.
    /// This function doesn't check existence of user, or test, caller must do so.
    /// If user or test not found, needed keys are cerated in etcd and filled with zeroes.
    pub async fn get_user_state_on_test(
        &self,
        user_id: &str,
        test_id: &str,
    ) -> Result<RustestUserState> {
        let points_key = format!("/rustest/user/{}/state/{}/points", user_id, test_id);
        let cur_round_key = format!("/rustest/user/{}/state/{}/cur_round", user_id, test_id);

        let resp = self
            .client
            .txn(
                TxnRequest::default()
                    .when_create_revision(KeyRange::key(points_key.clone()), TxnCmp::Greater, 0)
                    .and_then(RangeRequest::new(KeyRange::key(points_key.clone())))
                    .and_then(RangeRequest::new(KeyRange::key(cur_round_key.clone())))
                    .or_else(gen_put_request_with_serialized_value(points_key, &0)?)
                    .or_else(gen_put_request_with_serialized_value(cur_round_key, &0)?),
            )
            .await
            .context(format!(
                "cannot get user `{}` state for test `{}`",
                user_id, test_id
            ))?;

        if !resp.succeeded {
            // transaction failed, key wasn't present in storage, return default values
            return Ok(RustestUserState {
                points: 0,
                cur_round: 0,
            });
        }

        if resp.responses.len() != 2 {
            bail!(RustestStorageError::UnderlyingStorage {
                reason: "unexpected response on transaction request got, not enough responses"
                    .to_string(),
            });
        }

        let points = deserialize_txn_range_response::<u32>(resp.responses.get(0).unwrap())?;
        let cur_round = deserialize_txn_range_response::<u32>(resp.responses.get(1).unwrap())?;

        Ok(RustestUserState { points, cur_round })
    }

    /// Creates rustest and links it with author.
    /// Returns created rustest with filled ID field.
    /// User existence isn't checked, caller must do so.
    pub async fn create_rustest_with_author(
        &self,
        test: Rustest,
        author_id: &str,
    ) -> Result<Rustest> {
        let id = Uuid::new_v4().to_string();
        let test = Rustest {
            id: id.clone(),
            owner: author_id.to_string(),
            ..test
        };

        let rustest_path = format!("/rustest/rustests/{}", id);
        let user_rustest_path = format!("/rustest/user/{}/rustests/{}", author_id, id);

        let put_to_rustests = gen_put_request_with_serialized_value(&rustest_path, &test)?;
        let put_to_user_rustests =
            gen_put_request_with_serialized_value(&user_rustest_path, &test)?;

        let txn_response = self
            .client
            .txn(
                TxnRequest::default()
                    .when_create_revision(rustest_path.into(), TxnCmp::Greater, 0)
                    .or_else(put_to_rustests)
                    .or_else(put_to_user_rustests),
            )
            .await
            .context("cannot create rustest with author: cannot finish transaction to etcd")?;

        if txn_response.succeeded {
            // rustest existed
            bail!(RustestStorageError::Unexpected {
                reason: "cannot create rustest, cannot generate id for it".to_string()
            })
        }

        self.rustest_cache
            .lock()
            .await
            .put(test.id.clone(), test.clone());

        Ok(test)
    }

    /// Retrieves all rustests of given user. Doesn't check for user existence, caller must do so.
    pub async fn rustests_of_user(&self, user_id: &str) -> Result<Vec<Rustest>> {
        let path = format!("/rustest/user/{}/rustests/", user_id);
        let resp = self
            .client
            .get(
                RangeRequest::new(KeyRange::prefix(path))
                    .sort_by_create_revision(SortOrder::Descending),
            )
            .await
            .context("cannot get rustests of user from etcd")?;

        resp.kvs
            .into_iter()
            .map(|kvp| serde_json::from_slice::<Rustest>(&kvp.value).map_err(|err| anyhow!(err)))
            .collect()
    }

    /// Creates user and persists him to etcd storage.
    /// If user existed in etcd storage before, error returned.
    pub async fn create_user(&self, user: User) -> Result<User> {
        let user_key = format!("/rustest/users/{}", user.login);

        let txn_response = self
            .client
            .txn(
                TxnRequest::default()
                    .when_create_revision(KeyRange::key(user_key.clone()), TxnCmp::Greater, 0)
                    .or_else(gen_put_request_with_serialized_value(
                        user_key.clone(),
                        &user,
                    )?),
            )
            .await
            .context("cannot create user in etcd storage")?;

        if txn_response.succeeded {
            // value existed in etcd, so user was created already
            bail!(RustestStorageError::AlreadyExists { key: user_key });
        }

        self.user_cache
            .lock()
            .await
            .put(user.login.clone(), user.clone());

        Ok(user)
    }

    /// Retrieves user from etcd storage or cache. If user doesn't exist, returns None.
    pub async fn get_user(&self, user_id: &str) -> Result<Option<User>> {
        let mut cache_guard = self.user_cache.lock().await;
        if let Some(user) = cache_guard.get(user_id).cloned() {
            return Ok(Some(user));
        }

        let path = format!("/rustest/users/{}", user_id);
        let user: Option<User> = get_deserialized_value_by_key(&self.client, path)
            .await
            .map_err(|err| anyhow!(err))?;

        if let Some(user) = user {
            cache_guard.put(user_id.to_string(), user.clone());
            return Ok(Some(user));
        }

        Ok(None)
    }

    /// Get all users from etcd storage by `page` with `page_size`
    #[tracing::instrument(skip(self))]
    pub async fn get_users_by_page(
        &self,
        page: usize,
        page_size: usize,
    ) -> Result<(Vec<User>, usize)> {
        self.get_page_of_resources(
            page,
            page_size,
            &self.users_offset_to_revision,
            "/rustest/users/",
        )
        .await
        .context("cannot fetch users by page")
    }

    /// Get all rustests from etcd storage by `page` with `page_size`
    #[tracing::instrument(skip(self))]
    pub async fn get_rustests_by_page(
        &self,
        page: usize,
        page_size: usize,
    ) -> Result<(Vec<Rustest>, usize)> {
        self.get_page_of_resources(
            page,
            page_size,
            &self.rustests_offset_to_revision,
            "/rustest/rustests/",
        )
        .await
        .context("cannot fetch rustests by page")
    }

    async fn get_page_of_resources<T: DeserializeOwned>(
        &self,
        page: usize,
        page_size: usize,
        cache: &RwLock<HashMap<u64, i64>>,
        prefix: &str,
    ) -> Result<(Vec<T>, usize)> {
        let largest_revision = self
            .get_create_revision_for_offset(page_size * page, cache, prefix)
            .await
            .context("cannot get page of resources")?;

        let largest_revision = match largest_revision {
            Some(revision) => revision,
            None => return Ok((Vec::new(), 0)),
        };

        let page_resp = self
            .client
            .get(
                RangeRequest::new(KeyRange::prefix(prefix))
                    .min_create_revision(largest_revision + 1)
                    .sort_by_create_revision(SortOrder::Ascending)
                    .limit(page_size as u64),
            )
            .await
            .context("cannot get page of resources")?;

        let cnt = page_resp.count as usize;
        let resources: Result<Vec<T>> = parse_range_respone_to_dtos(page_resp);

        resources.map(|rustests| (rustests, cnt))
    }

    async fn get_create_revision_for_offset(
        &self,
        offset: usize,
        cache: &RwLock<HashMap<u64, i64>>,
        prefix: &str,
    ) -> Result<Option<i64>> {
        if offset == 0 {
            tracing::info!("got create revision for zero offset (lucky path)");
            return Ok(Some(-1));
        }

        let maybe_revision = cache.read().await.get(&(offset as u64)).cloned();

        if let Some(revision) = maybe_revision {
            tracing::info!("got create revision for offset from cache (lucky path)");
            return Ok(Some(revision));
        }

        tracing::info!("couldn't get create revision for offset from cache, going to etcd");
        let resp = self
            .client
            .get(
                RangeRequest::new(KeyRange::prefix(prefix))
                    .limit(offset as u64)
                    .keys_only()
                    .sort_by_create_revision(SortOrder::Ascending),
            )
            .await
            .context("cannot get largest revision for offset")?;

        let largest_create_revision = resp.kvs.last().map(|kv| kv.create_revision);
        if let Some(revision) = largest_create_revision {
            tracing::info!(
                "got revision for offset from etcd, putting it in cache for further usage"
            );

            cache.write().await.insert(offset as u64, revision);
            return Ok(Some(revision));
        }

        Ok(None)
    }

    pub async fn user_exists(&self, user_id: &str) -> Result<bool> {
        self.get_user(user_id)
            .await
            .context("cannot check user existence")
            .map(|maybe_user| maybe_user.is_some())
    }

    pub async fn rustest_exists(&self, test_id: &str) -> Result<bool> {
        self.get_rustest(test_id)
            .await
            .context("cannot check rustest existence")
            .map(|maybe_test| maybe_test.is_some())
    }

    pub async fn get_or_create_secret(&self, name: &str, secret_val: &str) -> Result<String> {
        let key = format!("/rustest/secrets/{}", name);
        let resp = self
            .client
            .txn(
                TxnRequest::default()
                    .when_create_revision(key.as_str().into(), TxnCmp::Greater, 0)
                    .and_then(RangeRequest::new(key.as_str().into()))
                    .or_else(PutRequest::new(key.as_str(), secret_val)),
            )
            .await
            .context(format!("cannot get or create secret `{}`", name))?;

        let txn_resp = resp.responses.first().ok_or(anyhow!(
            "unexpected etcd response, expected one range response for secret {}, got 0",
            name
        ))?;

        if let TxnOpResponse::Range(range_resp) = txn_resp {
            // range response -- key existed and value returned
            let kv = range_resp.kvs.first().ok_or(anyhow!("unexpected etcd response, expected one kv in range response for secret `{}`, but 0 found", name))?;
            Ok(kv.value_str().to_string())
        } else {
            // put response, key didn't exist, and value was set
            Ok(secret_val.to_string())
        }
    }
}

fn parse_range_respone_to_dtos<T: DeserializeOwned>(resp: RangeResponse) -> Result<Vec<T>> {
    resp.kvs
        .iter()
        .map(|kv| kv.value_str())
        .map(|val| serde_json::from_str::<T>(val).map_err(|err| anyhow!(err)))
        .collect()
}

fn gen_put_request_with_serialized_value<T: Serialize>(
    key: impl AsRef<str>,
    val: &T,
) -> Result<PutRequest> {
    let serialized_val = serde_json::to_string(val)?;
    Ok(PutRequest::new(key.as_ref(), serialized_val.as_str()))
}

fn deserialize_txn_range_response<T: DeserializeOwned>(resp: &TxnOpResponse) -> Result<T> {
    let val = match resp {
        TxnOpResponse::Range(range_resp) => {
            let kv = range_resp.kvs
                .first()
                .ok_or(RustestStorageError::UnderlyingStorage { reason: "unexpected empty response got in txn range response, maybe provided key wasn't present in storage".to_string() })?;

            serde_json::from_slice(&kv.value)
                .map_err(|err| RustestStorageError::Deserialization { reason: err.to_string() })?
        },
        _ => bail!(RustestStorageError::UnderlyingStorage{
            reason: "unexpected response on transaction request got: expected range responses, got unexpected ones".to_string()
        }),
    };

    Ok(val)
}

async fn get_deserialized_value_by_key<T: DeserializeOwned>(
    client: &Client,
    key: impl AsRef<str>,
) -> Result<Option<T>, RustestStorageError> {
    let resp =
        client
            .get(key.as_ref())
            .await
            .map_err(|err| RustestStorageError::UnderlyingStorage {
                reason: err.to_string(),
            })?;

    let value = resp.kvs.first().map(|kv| kv.value_str());

    let value = match value {
        Some(val) => val,
        None => return Ok(None),
    };

    serde_json::from_str(value).map_err(|err| RustestStorageError::Deserialization {
        reason: err.to_string(),
    })
}
