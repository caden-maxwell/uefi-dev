TMP_PART := /tmp/part.img
GNU_EFI := gnu-efi

IMG := OS.img
OVMF_SYS_DIR := /usr/share/OVMF
BOOT_DIR := boot
OVMF_DIR := OVMF

KERNELSRC := kernel
OBJDIR := $(KERNELSRC)/lib
BUILDDIR = $(KERNELSRC)/bin

LDS = $(KERNELSRC)/kernel.ld
CC = gcc
LD = ld

CFLAGS = -ffreestanding -fshort-wchar
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

# Recursive wildcard: https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

C_FILES = $(call rwildcard,$(KERNELSRC),*.c)          
OBJS = $(patsubst $(KERNELSRC)/%.c, $(OBJDIR)/%.o, $(C_FILES))
DIRS = $(wildcard $(KERNELSRC)/*)

all: $(IMG)

$(OBJDIR)/%.o: $(KERNELSRC)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

%.elf: $(OBJS)
	mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ $^
	
# Copy installed system OVMF files to the local directory
$(OVMF_DIR)/OVMF_CODE.fd $(OVMF_DIR)/OVMF_VARS.fd: $(OVMF_SYS_DIR)/OVMF_CODE.fd $(OVMF_SYS_DIR)/OVMF_VARS.fd
	@echo "Copying OVMF files..."
	mkdir -p OVMF
	cp $^ OVMF/

# Run the image in QEMU
run: $(IMG) $(OVMF_DIR)/OVMF_CODE.fd $(OVMF_DIR)/OVMF_VARS.fd
	@echo "Running $<..."
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,readonly=on,file=$(word 2,$^) \
		-drive if=pflash,format=raw,file=$(word 3,$^) \
		-drive if=ide,format=raw,file=$< \
		-net none

# Download and build GNU EFI
$(GNU_EFI)/.exists:
	git clone https://git.code.sf.net/p/gnu-efi/code $(GNU_EFI)
	$(MAKE) -C $(GNU_EFI) -s
	touch $@

# Build EFI application with GNU EFI
%.efi: %.c $(GNU_EFI)/.exists
	@echo "Building $@ from $<..."

	@# Patch the apps Makefile to build only our target
	sed -i '/^all:/ s/\$$(TARGETS)/$(notdir $@)/' $(GNU_EFI)/apps/Makefile
	
	@# Copy our source file to the apps directory
	cp $< $(GNU_EFI)/apps/$(notdir $<)

	$(MAKE) -C $(GNU_EFI) -s

	@# Copy EFI application to the current directory
	cp $(GNU_EFI)/x86_64/apps/$(notdir $@) $@

	@# Remove the source file from the apps directory
	rm -f $(GNU_EFI)/apps/$(notdir $<)

	@# Restore the apps Makefile
	sed -i '/^all:/ s/$(notdir $@)/$$(TARGETS)/' $(GNU_EFI)/apps/Makefile

# Create the image with the EFI application and startup script
$(IMG): $(BOOT_DIR)/main.efi $(BOOT_DIR)/startup.nsh $(KERNELSRC)/kernel.elf
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
	mcopy -i $(TMP_PART) $(word 3,$^) ::
	dd if=$(TMP_PART) of=$@ bs=512 count=91669 seek=2048 conv=notrunc

# Clean up build files
clean:
	rm -f *.o *.so boot/*.efi *.img kernel/*.elf

fresh: clean all

.PHONY: all clean run fresh
