FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Denver
ENV DISPLAY=:0
ENV QT_X11_NO_MITSHM=1

VOLUME /tmp/.X11-unix

# Install required packages
RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    qemu-system-x86 \
    mtools \
    parted \
    clang \
    lld

ARG BASE_DIR=/root/OS

RUN mkdir -p $BASE_DIR
WORKDIR $BASE_DIR

# copy src files into the container
COPY src/ .

# Build the project if needed
RUN make
