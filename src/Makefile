GNU_EFI := gnu-efi
TMP_PART := /tmp/part.img
IMG := uefi.img
OVMF_DIR := /usr/share/OVMF

.PHONY: all clean run rerun

all: $(IMG)

rerun: clean run

OVMF/OVMF_CODE.fd OVMF/OVMF_VARS.fd: $(OVMF_DIR)/OVMF_CODE.fd $(OVMF_DIR)/OVMF_VARS.fd
	@echo "Copying OVMF files..."
	mkdir -p OVMF
	cp $^ OVMF/

run: $(IMG) OVMF/OVMF_CODE.fd OVMF/OVMF_VARS.fd
	@echo "Running $<..."
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,readonly=on,file=OVMF/OVMF_CODE.fd \
		-drive if=pflash,format=raw,file=OVMF/OVMF_VARS.fd \
		-drive if=ide,format=raw,file=$< \
		-net none

$(GNU_EFI)/.exists:
	git clone https://git.code.sf.net/p/gnu-efi/code $(GNU_EFI)
	$(MAKE) -C $(GNU_EFI) -s
	touch $@

%.efi: %.c $(GNU_EFI)/.exists
	@echo "Building $@ from $<..."
	sed -i '/^TARGETS =/ s/$$/ $@/' $(GNU_EFI)/apps/Makefile
	
	cp $< $(GNU_EFI)/apps/$<
	$(MAKE) -C $(GNU_EFI) -s
	cp $(GNU_EFI)/x86_64/apps/$@ $@

	rm -f $(GNU_EFI)/apps/$<
	sed -i '/^TARGETS =/ s/ $@//' $(GNU_EFI)/apps/Makefile

$(IMG): main.efi startup.nsh
	@echo "Creating image $@..."
	dd if=/dev/zero of=$@ bs=512 count=93750
	parted $@ -s -a minimal mklabel gpt
	parted $@ -s -a minimal mkpart EFI FAT16 2048s 93716s
	parted $@ -s -a minimal toggle 1 boot
	dd if=/dev/zero of=$(TMP_PART) bs=512 count=91669
	mformat -i $(TMP_PART) -h 32 -t 32 -n 64 -c 1
	mmd -i $(TMP_PART) ::/EFI
	mmd -i $(TMP_PART) ::/EFI/BOOT
	mcopy -i $(TMP_PART) $< ::/EFI/BOOT
	mcopy -i $(TMP_PART) $(word 2,$^) ::
	dd if=$(TMP_PART) of=$@ bs=512 count=91669 seek=2048 conv=notrunc

clean:
	rm -f *.o *.so *.efi *.img
