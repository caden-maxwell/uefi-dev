# UEFI OS

Can be run in a docker container or natively.

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

Once you are up and running in the Docker container or have installed the requirements natively, you can run the following commands:

### `make` or `make all`:

Compiles the UEFI application and generates an image, which is be named `OS.img`.

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
