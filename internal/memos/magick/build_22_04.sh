#!/bin/sh

docker build -f Dockerfile_build_22_04 -t magick_build_22_04 .

id=$(docker create magick_build_22_04)

docker cp ${id}:/tmp/build/ImageMagick-7.1.1-21/utilities/magick magick_22_04
docker cp ${id}:/tmp/build/ImageMagick-7.1.1-21/policy.xml policy.xml

docker rm -v ${id}
