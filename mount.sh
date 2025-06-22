#!/usr/bin/env bash

IMG=$1
LOOP_FILE=$2
MNT_LINK=$3

if [ ! -f $LOOP_FILE ]; then
    echo "Mounting ${IMG} with udisksctl..."
    udisksctl loop-setup -f $IMG --no-user-interaction | grep -o '/dev/loop[0-9]*' | tee $LOOP_FILE

    LOOP=$(cat $LOOP_FILE)p1;
    udisksctl mount -b $LOOP

    MNT_PATH=$(lsblk -no MOUNTPOINT $LOOP)
    ln -vfs $MNT_PATH $MNT_LINK
else
    LOOP=$(cat $LOOP_FILE)p1;
    MNT_PATH=$(lsblk -no MOUNTPOINT $LOOP)
    echo "Loop device already created and mounted at $MNT_PATH"
fi