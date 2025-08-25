#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <image_file>"
    exit 1
fi

UDISKS2=udisksctl
if ! command -v "$UDISKS2" &> /dev/null; then
    echo \'$UDISKS2\' is not installed or is not in PATH. Exiting...
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
        "$UDISKS2" unmount -b "$LOOP_DEV"p1
    fi;
    
    # Remove loop device
    "$UDISKS2" loop-delete -b "$LOOP_DEV"
else
    echo "No loop device is setup for $IMG"
fi
