# UEFI Dev

This is a space to learn about UEFI development, specifically diving into the UEFI spec and how to create UEFI applications.

## Requirements:
- `make`
- `clang` or `x86_64-w64-mingw32-gcc`
- `lld`
- `qemu-system-x86_64`
- `mtools`
- `parted`
- `udisks2` *(optional, used for mounting/unmounting the image)*

## Usage:

Once the requirements are installed, the following commands can be run:

### `make [run]`:

Builds the BOOTX64.EFI application, creates a bootable image, and runs QEMU with the image and the included OVMF firmware.

### `make image`:

Builds the BOOTX64.EFI application and creates a bootable image.

### `make mount`:

Creates a loop device for the image and mounts it onto the local filesystem.  

### `make unmount`:

Unmounts the image and deletes the loop device associated with the image.  

### `make clean`:

Removes all `make`-generated files, and (if mounted) unmounts the boot image.
