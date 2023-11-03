#!/bin/sh

docker build -f Dockerfile_build_20_04 -t magick_build_20_04 .

id=$(docker create magick_build_20_04)

docker cp ${id}:/tmp/build/ImageMagick-7.1.1-21/utilities/magick magick_20_04

docker rm -v ${id}
