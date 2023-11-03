#!/bin/sh

docker build -f Dockerfile_build -t memos_build .

id=$(docker create memos_build)

docker cp ${id}:/tmp/build/memos .

docker rm -v ${id}
