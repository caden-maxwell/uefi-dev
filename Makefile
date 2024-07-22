GNU-EFI-HEADERS = gnu-efi/inc

.PHONY: all
all: uefi.img

.PHONY: run
run: uefi.img
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,readonly=on,file=ovmf/OVMF_CODE.fd \
		-drive if=pflash,format=raw,file=ovmf/OVMF_VARS.fd \
		-drive file=uefi.img,if=ide

main.o: main.c
	gcc $< \
		-c \
		-fno-stack-protector \
		-fpic \
		-fshort-wchar \
		-mno-red-zone \
		-I $(GNU-EFI-HEADERS) \
		-I $(GNU-EFI-HEADERS)/x86-64 \
		-DEFI_FUNCTION_WRAPPER \
		-o $@

main.so: main.o
	ld $< \
		gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o \
		-nostdlib \
		-znocombreloc \
		-T gnu-efi/gnuefi/elf_x86_64_efi.lds \
		-shared \
		-Bsymbolic \
		-L gnu-efi/x86_64/lib \
		-L gnu-efi/x86_64/gnuefi \
		-l:libgnuefi.a \
		-l:libefi.a \
		-o $@

main.efi: main.so
	objcopy -j .text \
		-j .sdata \
		-j .data \
		-j .dynamic \
		-j .dynsym \
		-j .rel \
		-j .rela \
		-j .reloc \
		--target=efi-app-x86_64 \
		$< \
		$@

uefi.img: main.efi
	dd if=/dev/zero of=$@ bs=512 count=93750
	parted $@ -s -a minimal mklabel gpt
	parted $@ -s -a minimal mkpart EFI FAT16 2048s 93716s
	parted $@ -s -a minimal toggle 1 boot
	dd if=/dev/zero of=/tmp/part.img bs=512 count=91669
	mformat -i /tmp/part.img -h 32 -t 32 -n 64 -c 1
	mcopy -i /tmp/part.img $< ::
	dd if=/tmp/part.img of=$@ bs=512 count=91669 seek=2048 conv=notrunc

.PHONY: clean
clean:
	rm -f *.o *.so *.efi *.img
