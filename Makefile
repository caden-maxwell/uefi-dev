TMP_PART = /tmp/part.img

.PHONY: all clean run

all: uefi.img

rerun: clean run

run: uefi.img
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,readonly=on,file=OVMF/OVMF_CODE.fd \
		-drive if=pflash,format=raw,file=OVMF/OVMF_VARS.fd \
		-drive if=ide,format=raw,file=$< \
		-net none \

gnu-efi/.git:
	git submodule update --init
	$(MAKE) -C gnu-efi -s

%.efi: %.c gnu-efi/.git
	sed -i '/^TARGETS =/ s/$$/ $@/' gnu-efi/apps/Makefile
	
	cp $< gnu-efi/apps/$<
	$(MAKE) -C gnu-efi -s
	cp gnu-efi/x86_64/apps/$@ $@

	rm -f gnu-efi/apps/$<
	sed -i '/^TARGETS =/ s/ $@//' gnu-efi/apps/Makefile

uefi.img: main.efi
	dd if=/dev/zero of=$@ bs=512 count=93750
	parted $@ -s -a minimal mklabel gpt
	parted $@ -s -a minimal mkpart EFI FAT16 2048s 93716s
	parted $@ -s -a minimal toggle 1 boot
	dd if=/dev/zero of=$(TMP_PART) bs=512 count=91669
	mformat -i $(TMP_PART) -h 32 -t 32 -n 64 -c 1
	mmd -i $(TMP_PART) ::/EFI
	mmd -i $(TMP_PART) ::/EFI/BOOT
	mcopy -i $(TMP_PART) $< ::/EFI/BOOT
	mcopy -i $(TMP_PART) startup.nsh ::
	dd if=$(TMP_PART) of=$@ bs=512 count=91669 seek=2048 conv=notrunc

clean:
	rm -f *.o *.so *.efi *.img
