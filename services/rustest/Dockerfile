FROM rust:1.72-slim-buster as builder

WORKDIR /
RUN apt-get update && \ 
    apt-get install -y protobuf-compiler && \ 
    USER=root cargo new --bin rustest

WORKDIR /rustest
COPY ./Cargo.* ./
RUN rm -rf .git* && \ 
    cargo build --release && \ 
    rm -rf ./src

COPY ./src ./src

RUN rm ./target/release/deps/rustest-* && \ 
    cargo build --release

FROM debian:buster-slim
ARG APP=/usr/src/app

RUN apt-get update \
    && apt-get install -y ca-certificates tzdata \
    && rm -rf /var/lib/apt/lists/*

ENV TZ=Etc/UTC \
    APP_USER=appuser

RUN groupadd $APP_USER \
    && useradd -g $APP_USER $APP_USER \
    && mkdir -p ${APP}

COPY --from=builder /rustest/target/release/rustest ${APP}/rustest

RUN chown -R $APP_USER:$APP_USER ${APP}

USER $APP_USER
WORKDIR ${APP}

CMD ["./rustest"]