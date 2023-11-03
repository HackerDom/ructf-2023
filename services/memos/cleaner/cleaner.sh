#!/bin/sh

while true; do
    date -uR

    find "/var/memos/storage/temp/" \
        -type f \
        -and -not -newermt "-1800 seconds" \
        -delete

    find "/var/memos/storage/drafts/" \
        -type f \
        -and -not -newermt "-1800 seconds" \
        -delete

    find "/var/memos/storage/images/" \
        -type f \
        -and -not -newermt "-1800 seconds" \
        -delete

    find "/var/memos/storage/passwords/" \
        -type f \
        -and -not -newermt "-1800 seconds" \
        -delete

    sleep 60
done
