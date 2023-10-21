FROM golang:1.21.3-alpine3.18

COPY . /app
WORKDIR /app/server
RUN go build

FROM golang:1.21.3-alpine3.18

WORKDIR /
COPY --from=0 /app/server/config.json /config.json
COPY --from=0 /app/server/server /server

ENTRYPOINT ["./server", "--config", "config.json"]
