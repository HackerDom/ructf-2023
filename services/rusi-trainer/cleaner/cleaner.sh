#!/bin/sh
while true; do
    date -uR
    mysql -h rusi-db -u user --password=password mydatabase -e "DELETE FROM user WHERE createdDate < NOW() - INTERVAL 40 MINUTE;"
    sleep 30
done