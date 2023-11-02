#!/bin/sh

while true; do
    date -uR

    find "/var/perune/database/" \
        -type f \
        -and -not -newermt "-1800 seconds" \
        -delete

    sleep 60
done