ARCH = x86_64
TARGET = kernel.elf
SRCS = kernel.c
OUTDIR = bin

all: $(TARGET)

ifeq ($(USE_GCC),)
CC = clang -target $(ARCH)-elf
LD = ld.lld
else
CC = $(ARCH)-elf-gcc
LD = $(ARCH)-elf-ld
endif

# UEFI wastes lots and lots of memory. Link our "kernel" at an address (32M) which isn't used by UEFI
$(TARGET): $(SRCS)
	@mkdir -p $(OUTDIR)
	$(CC) -ffreestanding -fno-stack-protector -fno-stack-check -D__$(ARCH)__ -I. -c $< -o $(OUTDIR)/kernel.o
	$(LD) -nostdlib -z max-page-size=0x1000 -Ttext=0x01000000 $(OUTDIR)/kernel.o -o $(OUTDIR)/$(TARGET)

clean:
	rm -rf $(OUTDIR)
