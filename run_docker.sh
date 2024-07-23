#!/usr/bin/env bash

docker run \
    --rm -it \
    --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
    os