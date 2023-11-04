FROM ubuntu:22.04

RUN apt update && apt install -y socat libfreetype6 libjpeg9 libxml2 ghostscript

WORKDIR /var/memos

ENV MAGICK_CONFIGURE_PATH=/var/memos/magick

COPY magick ./magick
COPY memos entrypoint.sh index.html ./

RUN mkdir -p storage/temp storage/drafts storage/images storage/passwords

RUN chmod +x memos entrypoint.sh magick/magick \
    && chown -R nobody:nogroup storage

USER nobody

ENTRYPOINT [ "./entrypoint.sh" ]
