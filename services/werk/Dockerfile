FROM golang:1.21.3-alpine3.17

COPY . /app
WORKDIR /app

RUN go build .

CMD ["./werk", "-config", "game_config.json"]
