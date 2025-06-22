#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <image_file>"
    exit 1
fi

IMG="$1"

# Ensure the loop device actually exists for this image
LOOP_DEV=$(losetup -j "$IMG" | grep -o '/dev/loop[0-9]*')
if [ -n "$LOOP_DEV" ]; then
    echo "Unmounting $LOOP_DEV with udisksctl..."

    # Unmount partition if it is still mounted
    MNT_PATH=$(lsblk -no MOUNTPOINT "$LOOP_DEV"p1)
    if [ -n "$MNT_PATH" ]; then
        udisksctl unmount -b "$LOOP_DEV"p1
    fi;
    
    # Remove loop device
    udisksctl loop-delete -b "$LOOP_DEV"
else
    echo "No loop device is setup for $IMG"
fi
