Requirements:
- `qemu`
- `ovmf`
- `mtools`

To initialize repo, run `./init.sh`

Copy both `OVMF_CODE.fd` and `OVMF_VARS.fd` from `/usr/share/OVMF` to `./OVMF`

Then do `make run`
