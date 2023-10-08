FROM alpine:latest

RUN apk update && apk add socat

WORKDIR /var/perune

COPY perune entrypoint.sh ./

RUN mkdir database

RUN chmod +x perune entrypoint.sh \
    && chown nobody:nogroup database

USER nobody

ENTRYPOINT [ "./entrypoint.sh" ]
