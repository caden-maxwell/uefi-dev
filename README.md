Requirements:
- `qemu`
- `ovmf`
- `mtools`

To initialize repo, run `./init.sh`

Copy (don't move) both `OVMF_CODE.fd` and `OVMF_VARS.fd` from `/usr/share/OVMF` to `./ovmf`

Then do `make run`
