#![forbid(unsafe_code)]
pub mod application;
pub mod authenticator;
pub mod constants;
pub mod dto;
pub mod routes;
pub mod storage;

use anyhow::{anyhow, Result};

use axum::ServiceExt;
use etcd_rs::{Client, ClientConfig};
use tower::Layer;
use tower_http::normalize_path::NormalizePathLayer;
use tracing_subscriber::{
    filter, prelude::__tracing_subscriber_SubscriberExt, util::SubscriberInitExt,
};

use std::{env, net::SocketAddr, sync::Arc, time::Duration};
use tracing::Level;

use crate::{
    application::RussApplication, authenticator::Authenticator, routes::AppState,
    storage::ETCDRustestStorage,
};

#[tokio::main]
async fn main() -> Result<()> {
    let etcd_url = env::var("ETCD_HOST")
        .ok()
        .unwrap_or("http://localhost:2379".to_string());

    let client = Client::connect(
        ClientConfig::new([etcd_url.into()]).connect_timeout(Duration::from_secs(5)),
    )
    .await?;

    let mut storage = ETCDRustestStorage::new(client);
    tracing::info!("started cache warming");
    storage.warm_caches(20).await?;
    tracing::info!("cache warmed successfully");
    let storage = Arc::new(storage);

    let rus_app = Arc::new(RussApplication::new(Arc::clone(&storage)));

    let mut authenticator = Authenticator::new(Arc::clone(&storage));
    tracing::info!("started initialization of authenticator");
    authenticator.init().await?;
    tracing::info!("authenticator warmed successfuly");

    let authenticator = Arc::new(authenticator);

    let state = AppState::new(rus_app, authenticator);

    let filter = filter::Targets::new()
        .with_target("", Level::INFO)
        .with_target("rustest", Level::DEBUG)
        .with_target("tower_http", Level::INFO)
        .with_target("tower_http::trace::make_span", Level::DEBUG);

    let tracing_layer = tracing_subscriber::fmt::layer();

    tracing_subscriber::registry()
        .with(tracing_layer)
        .with(filter)
        .init();

    // build our application with a route
    let app = routes::build_router(state);
    let normalized_app = NormalizePathLayer::trim_trailing_slash().layer(app);

    let addr = SocketAddr::from(([0, 0, 0, 0], 3000));
    tracing::info!("listening on {}", addr);
    axum::Server::bind(&addr)
        .serve(normalized_app.into_make_service())
        .await
        .map_err(|err| anyhow!(err))
}
