#!/usr/bin/env bash

LOOP_FILE=$1
if [ -f $LOOP_FILE ]; then
    LOOP_DEV=$(grep -o '/dev/loop[0-9]*' $LOOP_FILE)
    echo "Unmounting $LOOP_DEV..."
    MNT_PATH=$(lsblk -no MOUNTPOINT "$LOOP_DEV"p1)
    if [ -n "$MNT_PATH" ]; then
        udisksctl unmount -b "$LOOP_DEV"p1
    fi;
    udisksctl loop-delete -b $LOOP_DEV
    rm -f $LOOP_FILE
else
    echo "No loop device to unmount."
fi
