FROM golang:1.21.1 AS build

WORKDIR /build

COPY ./go.mod ./go.sum /build
RUN go mod download

COPY ./pkg /build/pkg

RUN CGO_ENABLED=0 GOOS=linux go build -o sqlfs /build/pkg/main.go

FROM alpine:3.18.4

WORKDIR /app

RUN apk add fuse

RUN mkdir /mnt/sqlfs
RUN mkdir /loopbackfs

COPY --from=build /build/sqlfs /app/sqlfs

CMD ["/app/sqlfs", "/mnt/sqlfs", "/loopbackfs"]
