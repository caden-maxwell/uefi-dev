#!/usr/bin/env bash

docker run \
    --rm -it --cpus="2.0" --memory="4g" \
    --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
    os
