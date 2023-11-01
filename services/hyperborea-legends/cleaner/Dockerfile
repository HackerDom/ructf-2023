FROM alpine:3.15

RUN apk --no-cache add postgresql12-client

COPY cleaner.sh .
RUN chmod +x cleaner.sh

ENTRYPOINT [ "/cleaner.sh" ]
