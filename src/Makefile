TMP_PART := /tmp/part.img
GNU_EFI := gnu-efi

IMG := OS.img
QEMU_DIR := /usr/share/qemu
BOOTDIR := boot
BOOT_BIN := $(BOOTDIR)/bin

KERNDIR := kernel
KERN_OBJDIR := $(KERNDIR)/lib
KERN_BUILDDIR = $(KERNDIR)/bin

LDS = $(KERNDIR)/kernel.ld
CC = x86_64-w64-mingw32-gcc
LD = ld

CFLAGS = -ffreestanding -fshort-wchar
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

# Recursive wildcard: https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

C_FILES = $(call rwildcard,$(KERNDIR),*.c)          
OBJS = $(patsubst $(KERNDIR)/%.c, $(KERN_OBJDIR)/%.o, $(C_FILES))
DIRS = $(wildcard $(KERNDIR)/*)

all: $(IMG)

$(KERN_OBJDIR)/%.o: $(KERNDIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

%.elf: $(OBJS)
	mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ $^

# Run the image in QEMU
run: $(IMG) $(QEMU_DIR)/OVMF.fd
	@echo "Running $<..."
	qemu-system-x86_64 -cpu qemu64 \
		-bios $(word 2,$^) \
		-drive if=ide,format=raw,file=$< \
		-net none \
		--serial file:./out.log

# Download and build GNU EFI
$(GNU_EFI)/.exists:
	git clone https://github.com/ncroxon/gnu-efi $(GNU_EFI)
	touch $@

# Build EFI application with GNU EFI
$(BOOT_BIN)/main.efi: $(BOOTDIR)/main.c $(GNU_EFI)/.exists
	@echo "Building $@ from $<..."
	mkdir -p $(@D)

	@# Patch the apps Makefile to build only our target
	sed -i '/^all:/ s/\$$(TARGETS)/$(notdir $@)/' $(GNU_EFI)/apps/Makefile
	
	@# Copy our source file to the apps directory
	cp $< $(GNU_EFI)/apps/$(notdir $<)

	$(MAKE) -C $(GNU_EFI) -s CC=$(CC) apps

	@# Copy EFI application to the current directory
	cp $(GNU_EFI)/x86_64/apps/$(notdir $@) $@

	@# Remove the source file from the apps directory
	rm -f $(GNU_EFI)/apps/$(notdir $<)

	@# Remove obj files from x86_64/apps
	rm -f $(GNU_EFI)/x86_64/apps/*.o
	rm -f $(GNU_EFI)/x86_64/apps/*.so

	@# Restore the apps Makefile
	sed -i '/^all:/ s/$(notdir $@)/$$(TARGETS)/' $(GNU_EFI)/apps/Makefile

# Create the image with the EFI application and startup script
$(IMG): $(BOOT_BIN)/main.efi $(KERN_BUILDDIR)/kernel.elf
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
	rm -rf *.o *.so *.img $(BOOT_BIN) $(KERN_OBJDIR) $(KERN_BUILDDIR) *.log

fresh: clean all

.PHONY: all clean run fresh
