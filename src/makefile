IMG := OS.img

BOOT := boot
BOOT_BIN := $(BOOT)/bin

KERNEL := kernel
KERNEL_BIN := $(KERNEL)/bin

TMP_PART := /tmp/part.img
LOG := out.log

.PHONY: all run clean nuke fresh 

all: $(IMG)

# Build EFI application with POSIX-EFI
$(BOOT_BIN)/%.efi: $(BOOT)/%.c
	$(MAKE) -C $(BOOT)

$(KERNEL_BIN)/kernel.elf: $(wildcard $(KERNEL)/*.c)
	$(MAKE) -C $(KERNEL)

# Create the image with the EFI application and startup script
$(IMG): $(BOOT_BIN)/main.efi $(KERNEL_BIN)/kernel.elf
	@echo "Creating image $@..."
	dd if=/dev/zero of=$@ bs=512 count=93750
	parted $@ -s mklabel gpt
	parted $@ -s mkpart EFI FAT32 2048s 93716s
	parted $@ -s toggle 1 boot
	dd if=/dev/zero of=$(TMP_PART) bs=512 count=91669
	mformat -i $(TMP_PART) -h 32 -t 32 -n 64 -c 1
	mmd -i $(TMP_PART) ::/EFI
	mmd -i $(TMP_PART) ::/EFI/BOOT
	mcopy -i $(TMP_PART) $< ::/EFI/BOOT/bootx64.efi
	mcopy -i $(TMP_PART) $(KERNEL_BIN)/kernel.elf ::
	dd if=$(TMP_PART) of=$@ bs=512 count=91669 seek=2048 conv=notrunc

# Run the image in QEMU
run: $(IMG) OVMF.fd
	@echo "Running $<..."
	qemu-system-x86_64 -cpu qemu64 -m 256M \
		-bios $(word 2,$^) \
		-drive format=raw,file=$< \
		-net none --serial file:$(LOG)

# Clean files and rebuild
fresh: clean all

# Clean up build files
clean nuke:
	rm -f $(IMG) $(LOG)
	$(MAKE) -C $(BOOT) $@
	$(MAKE) -C $(KERNEL) $@
