# UEFI Dev

I've never really understood the difference between things like BIOS, UEFI, bootloaders, and the kernel, or really much of anything that goes on when a computer boots. This is my space to learn about all of that, specifically diving into the UEFI spec and how to create UEFI applications. In the future, maybe I'll even attempt to write my own kernel and mini OS. We'll see!

## Requirements:
- `make`
- `clang`
- `lld`
- `qemu-system-x86_64`
- `mtools`
- `parted`

## `Makefile` usage:

Once you have these installed, you can run the following commands:

### `make [all]`:

Compiles the UEFI application, and generates an image named `OS.img`.

### `make run`:

Runs the image in QEMU with the required OVMF firmware.

### `make clean`:

Removes all generated files. Files removed include:
- `*.img`
- `*.efi`
- `*.so`
- `*.o`

### `make fresh`:

This makes a fresh build, which is equivalent to running:
```bash
$ make clean
$ make
```
