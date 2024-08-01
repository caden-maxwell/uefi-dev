# UEFI Dev

I've never really understood the difference between things like BIOS, UEFI, bootloaders, and the kernel, or really much of anything that goes on when a computer boots. This is my space to learn about all of that, specifically diving into the UEFI spec and how to create UEFI applications. In the future, maybe I'll even attempt to write my own kernel and mini OS. We'll see!

This code can be run in via [Docker](#Docker) or [natively](#Native).

## Docker

### Requirements:
- `docker`

### Usage:

```bash
$ ./build_run_docker.sh
```
To run the container again without rebuilding:

```bash
$ ./run_docker.sh
```

## Native

### Requirements:
- `build-essential` 
- `qemu-system` 
- `ovmf` 
- `mtools` 
- `parted`

## `Makefile` usage:

Once you are up and running in the Docker or have installed the requirements natively, you can run the following commands:

### `make` or `make all`:

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
