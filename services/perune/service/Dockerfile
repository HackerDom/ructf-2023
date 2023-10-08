FROM alpine:latest

RUN apk update && apk add socat

WORKDIR /var/perune

COPY --chown=nobody:nogroup perune entrypoint.sh ./

RUN mkdir database

RUN chmod +x perune entrypoint.sh \
    && chown nobody:nogroup database

USER nobody

ENTRYPOINT [ "./entrypoint.sh" ]
