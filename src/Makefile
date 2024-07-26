IMG := OS.img
BOOT := boot
KERNEL := kernel

POSIX_UEFI := $(BOOT)/uefi
BOOT_BIN := $(BOOT)/bin

QEMU_DIR := /usr/share/qemu
TMP_PART := /tmp/part.img

all: $(IMG)

# Run the image in QEMU
run: $(IMG) $(QEMU_DIR)/OVMF.fd
	@echo "Running $<..."
	qemu-system-x86_64 -cpu qemu64 \
		-bios $(word 2,$^) \
		-drive if=ide,format=raw,file=$< \
		-net none \
		--serial file:./out.log

# Get POSIX-UEFI
$(POSIX_UEFI):
	git clone -n https://gitlab.com/bztsrc/posix-uefi.git $(@D)/posix-uefi
	cd $(@D)/posix-uefi \
		&& git sparse-checkout set --no-cone '/uefi/*' \
		&& git checkout
	
	@# Move the inner directories to the root
	mv $(@D)/posix-uefi/uefi $(@D)
	rm -rf $(@D)/posix-uefi

# Build EFI application with GNU EFI
$(BOOT_BIN)/%.efi: $(BOOT)/%.c $(POSIX_UEFI)
	$(MAKE) -C $(BOOT)

$(KERNEL):
	$(MAKE) -C $(KERNEL)

# Create the image with the EFI application and startup script
$(IMG): $(BOOT_BIN)/main.efi $(KERNEL)
	@echo "Creating image $@..."
	dd if=/dev/zero of=$@ bs=512 count=93750
	parted $@ -s -a minimal mklabel gpt
	parted $@ -s -a minimal mkpart EFI FAT16 2048s 93716s
	parted $@ -s -a minimal toggle 1 boot
	dd if=/dev/zero of=$(TMP_PART) bs=512 count=91669
	mformat -i $(TMP_PART) -h 32 -t 32 -n 64 -c 1
	mmd -i $(TMP_PART) ::/EFI
	mmd -i $(TMP_PART) ::/EFI/BOOT
	mcopy -i $(TMP_PART) $< ::/EFI/BOOT/bootx64.efi
	mcopy -i $(TMP_PART) $(word 2,$^) ::
	dd if=$(TMP_PART) of=$@ bs=512 count=91669 seek=2048 conv=notrunc

# Clean up build files
clean:
	rm -rf *.o *.so *.img *.log
	$(MAKE) -C $(POSIX_UEFI) clean

# Remove all build files and git subdir
distclean: clean
	rm -rf $(POSIX_UEFI)

# Clean files and rebuild
fresh: clean all

.PHONY: all clean run fresh
