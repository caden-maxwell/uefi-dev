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

### Create `uefi.img`:
```bash
$ make # or
$ make all
```

### Start QEMU with UEFI image:
```bash
$ make run
```

### Clean up generated files:
```bash
$ make clean
```

### Fresh build:
```bash
$ make fresh
```
which is equivalent to:
```bash
$ make clean
$ make
```
