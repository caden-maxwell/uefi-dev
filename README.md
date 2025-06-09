# UEFI Dev

This is a space to learn about UEFI development, specifically diving into the UEFI spec and how to create UEFI applications.

## Requirements:
- `make`
- `clang` or `x86_64-w64-mingw32-gcc`
- `lld`
- `qemu-system-x86_64`
- `mtools`
- `parted`

## Usage:

Once you have these installed, you can run the following commands:

### `make [run]`:

Runs the image in QEMU with the required OVMF firmware.

### `make clean`:

Removes all `make`-generated files.
