#![forbid(unsafe_code)]
use std::sync::Arc;

use aide::axum::ApiRouter;
use aide::{
    axum::routing::{get, post},
    openapi::{Info, OpenApi},
};

use axum::{
    debug_handler,
    extract::{Path, Query, State},
    headers::{authorization::Bearer, Authorization},
    http::Request,
    middleware::{self, Next},
    response::Response,
    Extension, Json, Router, TypedHeader,
};

use axum_trace_id::{SetTraceIdLayer, TraceId};
use tower_http::services::{ServeDir, ServeFile};
use tower_http::{
    cors::{Any, CorsLayer},
    normalize_path::NormalizePathLayer,
    trace::{DefaultOnFailure, DefaultOnRequest, DefaultOnResponse, TraceLayer},
};
use tracing::Level;

use crate::{
    application::{
        CreateRustestRequest, CreateRustestResponse, GetRustestRequest, GetRustestResponse,
        GetRustestsRequest, GetRustestsResponse, GetUsersRequest, GetUsersResponse,
        RussApplication, RustestApplicationError, RustestUserStateRequest,
        RustestUserStateResponse, SubmitAnswerRequest, SubmitAnswerResponse, UserRustestsRequest,
        UserRustestsResponse,
    },
    authenticator::{
        AuthenticationError, Authenticator, RegisterUserRequest, RegisterUserResponse,
        UserLoginRequest, UserLoginResponse,
    },
    dto::Rustest,
};

#[derive(Clone)]
pub struct AppState {
    rus_app: Arc<RussApplication>,
    authenticator: Arc<Authenticator>,
}

impl AppState {
    pub fn new(rus_app: Arc<RussApplication>, authenticator: Arc<Authenticator>) -> Self {
        Self {
            rus_app,
            authenticator,
        }
    }
}

pub fn build_router(state: AppState) -> Router {
    let authenticated_routes = ApiRouter::new()
        .api_route("/rustest/:id/preview", get(get_rustest)) // get rustest with id for current user
        .api_route("/rustest/:id/solve", get(get_rustest_state)) // get rustest with id for current user
        .api_route("/rustest/:id/submit", post(submit_answer))
        .api_route("/rustest", post(create_rustest)) // create rustest
        .api_route("/my_rustests", get(get_my_rustests)) // get rustest of current user
        .api_route("/user_rustests/:user_id", get(get_user_rustests))
        .api_route("/users", get(get_all_users_by_page))
        .api_route("/rustests", get(get_all_rustests_by_page))
        .route_layer(middleware::from_fn_with_state(state.clone(), auth));

    let unauthenticated_routes = ApiRouter::new()
        .api_route("/login", post(user_login))
        .api_route("/register", post(user_registration))
        .route("/schema", get(openapi_schema));

    let mut api = OpenApi {
        info: Info {
            title: "Openapi schema for rustest application".to_string(),
            ..Default::default()
        },
        ..Default::default()
    };

    let merged_api = ApiRouter::new()
        .merge(authenticated_routes)
        .merge(unauthenticated_routes);

    let serve_dir =
        ServeDir::new("front/build").not_found_service(ServeFile::new("front/build/index.html"));

    ApiRouter::new()
        .nest("/api", merged_api)
        .nest_service("/", serve_dir)
        .finish_api(&mut api)
        .layer(Extension(api))
        .layer(
            TraceLayer::new_for_http()
                .make_span_with(|request: &Request<_>| {
                    let trace_id = request.extensions().get::<TraceId<String>>().unwrap();
                    tracing::info_span!("http_request", trace_id = trace_id.id)
                })
                .on_request(DefaultOnRequest::new().level(Level::INFO))
                .on_response(DefaultOnResponse::new().level(Level::INFO))
                .on_failure(DefaultOnFailure::new().level(Level::ERROR)),
        )
        .layer(NormalizePathLayer::trim_trailing_slash())
        .layer(
            CorsLayer::new()
                .allow_methods(Any)
                .allow_origin(Any)
                .allow_headers(Any),
        )
        .layer(SetTraceIdLayer::<String>::new())
        .with_state(state)
}

#[derive(Clone)]
struct AuthenticatedUser {
    login: String,
}

fn trace_err<T: ToString>(err: T) -> T {
    tracing::error!(error = err.to_string());
    err
}

async fn auth<B>(
    State(state): State<AppState>,
    TypedHeader(bearer): TypedHeader<Authorization<Bearer>>,
    mut request: Request<B>,
    next: Next<B>,
) -> Result<Response, AuthenticationError> {
    let login = state
        .authenticator
        .validate_token(bearer.token())
        .await
        .map_err(trace_err)?;
    request.extensions_mut().insert(AuthenticatedUser { login });

    Ok(next.run(request).await)
}

#[debug_handler]
async fn openapi_schema(Extension(api): Extension<OpenApi>) -> Json<OpenApi> {
    Json(api)
}

#[debug_handler]
async fn get_rustest_state(
    Path(test_id): Path<String>,
    Extension(user): Extension<AuthenticatedUser>,
    State(state): State<AppState>,
) -> Result<Json<RustestUserStateResponse>, RustestApplicationError> {
    state
        .rus_app
        .get_rustest_user_state(RustestUserStateRequest {
            user_id: user.login,
            test_id,
        })
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn submit_answer(
    Path(test_id): Path<String>,
    Extension(user): Extension<AuthenticatedUser>,
    State(state): State<AppState>,
    Json(req): Json<SubmitAnswerRequest>,
) -> Result<Json<SubmitAnswerResponse>, RustestApplicationError> {
    let req = SubmitAnswerRequest {
        user_id: user.login,
        test_id,
        ..req
    };

    state
        .rus_app
        .submit_answer(req)
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn user_registration(
    State(state): State<AppState>,
    Json(req): Json<RegisterUserRequest>,
) -> Result<Json<RegisterUserResponse>, AuthenticationError> {
    req.validate()?;

    state
        .authenticator
        .register(req)
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn user_login(
    State(state): State<AppState>,
    Json(req): Json<UserLoginRequest>,
) -> Result<Json<UserLoginResponse>, AuthenticationError> {
    req.validate()?;

    state
        .authenticator
        .authenticate(req)
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn create_rustest(
    State(state): State<AppState>,
    Extension(user): Extension<AuthenticatedUser>,
    Json(req): Json<CreateRustestRequest>,
) -> Result<Json<CreateRustestResponse>, RustestApplicationError> {
    let req = CreateRustestRequest {
        user_id: user.login.clone(),
        rustest: Rustest {
            owner: user.login,
            ..req.rustest
        },
    };

    state
        .rus_app
        .create_rustest(req)
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn get_rustest(
    State(state): State<AppState>,
    Extension(user): Extension<AuthenticatedUser>,
    Path(test_id): Path<String>,
) -> Result<Json<GetRustestResponse>, RustestApplicationError> {
    state
        .rus_app
        .get_rustest(GetRustestRequest {
            test_id,
            user_id: user.login,
        })
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn get_my_rustests(
    State(state): State<AppState>,
    Extension(user): Extension<AuthenticatedUser>,
) -> Result<Json<UserRustestsResponse>, RustestApplicationError> {
    state
        .rus_app
        .get_users_rustests(UserRustestsRequest {
            user_id: user.login,
        })
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn get_user_rustests(
    Path(user_id): Path<String>,
    State(state): State<AppState>,
    Extension(_user): Extension<AuthenticatedUser>,
) -> Result<Json<UserRustestsResponse>, RustestApplicationError> {
    state
        .rus_app
        .get_users_rustests(UserRustestsRequest { user_id })
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn get_all_users_by_page(
    State(state): State<AppState>,
    Extension(_user): Extension<AuthenticatedUser>,
    Query(req): Query<GetUsersRequest>,
) -> Result<Json<GetUsersResponse>, RustestApplicationError> {
    state
        .rus_app
        .get_all_users_by_page(req)
        .await
        .map(jsonify)
        .map_err(trace_err)
}

#[debug_handler]
async fn get_all_rustests_by_page(
    State(state): State<AppState>,
    Extension(_user): Extension<AuthenticatedUser>,
    Query(req): Query<GetRustestsRequest>,
) -> Result<Json<GetRustestsResponse>, RustestApplicationError> {
    state
        .rus_app
        .get_all_rustests_by_page(req)
        .await
        .map(jsonify)
        .map_err(trace_err)
}

fn jsonify<T>(resp: T) -> Json<T> {
    Json(resp)
}
