#!/bin/sh

docker build -f Dockerfile_build -t magick_build .

id=$(docker create magick_build)

docker cp ${id}:/tmp/build/ImageMagick-7.1.1-21/utilities/magick magick
docker cp ${id}:/tmp/build/ImageMagick-7.1.1-21/policy.xml policy.xml

docker rm -v ${id}
