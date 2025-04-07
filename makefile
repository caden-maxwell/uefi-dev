SOURCES = efi.c efilib.c
OBJS    = $(SOURCES:.c=.o)
DEPENDS = $(OBJS:.o=.d)
TARGET  = BOOTX64.EFI

IMG := OS.img
TMP_PART = /tmp/part.img
FIRMWARE_BIN = bios64.bin
QEMU_LOG = qemu.log

#CC = x86_64-w64-mingw32-gcc
#LDFLAGS = \
 	-nostdlib \
  	-Wl,--subsystem,10 \
  	-e UefiEntry

CC = clang -target x86_64-unknown-windows
LDFLAGS = \
	-nostdlib \
	-target x86_64-unknown-windows \
	-fuse-ld=lld-link \
	-Wl,-subsystem:efi_application \
	-Wl,-entry:UefiEntry

CFLAGS = \
	-std=c17 \
	-Wall -Wextra -Wpedantic \
	-mno-red-zone \
	-MMD \
	-ffreestanding

.PHONY: all run clean nuke fresh 

all: $(IMG)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $? -o $@

-include $(DEPENDS)

# Create the image with the EFI application and startup script
$(IMG): $(TARGET)
	@echo "Creating image $@..."
	dd if=/dev/zero of=$@ bs=512 count=93750
	parted $@ -s -a minimal mklabel gpt
	parted $@ -s -a minimal mkpart EFI FAT32 2048s 93716s
	parted $@ -s -a minimal toggle 1 boot
	dd if=/dev/zero of=$(TMP_PART) bs=512 count=91669
	mformat -i $(TMP_PART) -h 32 -t 32 -n 64 -c 1
	mmd -i $(TMP_PART) ::/EFI
	mmd -i $(TMP_PART) ::/EFI/BOOT
	mcopy -i $(TMP_PART) $< ::/EFI/BOOT/$(TARGET)
	dd if=$(TMP_PART) of=$@ bs=512 count=91669 seek=2048 conv=notrunc

run: $(IMG)
	@echo "Running $<..."
	qemu-system-x86_64 \
		-drive format=raw,file=$< \
		-bios $(FIRMWARE_BIN) \
		-m 256M \
		-machine q35 \
		-net none \
		--serial file:$(QEMU_LOG)

fresh: clean all

clean:
	rm -f *.img *.EFI *.log *.o *.d
