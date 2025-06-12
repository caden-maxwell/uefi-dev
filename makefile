SRC_DIR := src
INC_DIR := include
BUILD_DIR := build

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDS := $(OBJS:.o=.d)
TARGET  := $(BUILD_DIR)/BOOTX64.EFI

IMG := $(BUILD_DIR)/OS.img
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
	-ffreestanding \
	-I$(INC_DIR)

.PHONY: run clean image

run: $(IMG)
	@echo "Running $<..."
	qemu-system-x86_64 \
		-drive format=raw,file=$< \
		-bios $(FIRMWARE_BIN) \
		-m 256M \
		-machine q35 \
		-net none \
		--serial file:$(QEMU_LOG)

image: $(IMG)

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
	mcopy -i $(TMP_PART) $< ::/EFI/BOOT/$(notdir $(TARGET))
	dd if=$(TMP_PART) of=$@ bs=512 count=91669 seek=2048 conv=notrunc

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

-include $(DEPENDS)

clean:
	rm -rf *.log $(BUILD_DIR)
