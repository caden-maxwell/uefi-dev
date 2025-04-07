# UEFI Dev

I've never really understood the difference between things like BIOS, UEFI, bootloaders, and the kernel, or really much of anything that goes on when a computer boots. This is my space to learn about all of that, specifically diving into the UEFI spec and how to create UEFI applications. In the future, maybe I'll even attempt to write my own kernel and mini OS. We'll see!

## Requirements:
- `make`
- `clang` or `x86_64-w64-mingw32-gcc`
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

Removes all generated files.

### `make fresh`:

Equivalent to:
```bash
$ make clean all
```
