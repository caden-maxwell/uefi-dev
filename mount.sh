#!/usr/bin/env bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <image_file> <mount_symlink>"
    exit 1
fi

IMG="$1"
MNT_LINK="$2"

# Ensure loop device for this image does not already exist
LOOP_DEV=$(losetup -j "$IMG" | grep -o '/dev/loop[0-9]*')
if [ -z "$LOOP_DEV" ]; then
    echo "Creating loop device for ${IMG} and mounting partition with udisksctl..."

    # Setup loop device and mount the first partition
    PART=$(udisksctl loop-setup -f "$IMG" --no-user-interaction | grep -o '/dev/loop[0-9]*')p1
    udisksctl mount -b "$PART"

    # Make a symlink to the mount for ease of use
    echo Setting up symlink...
    MNT_PATH=$(lsblk -no MOUNTPOINT "$PART")
    ln -vfs "$MNT_PATH" "$MNT_LINK"
else
    MNT_PATH=$(lsblk -no MOUNTPOINT ${LOOP_DEV}p1)
    echo "Loop device already setup for $IMG"
    echo "${LOOP_DEV}p1 mounted at $MNT_PATH"
fi
