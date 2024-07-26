ARCH = x86_64
TARGET = kernel.elf
SRCS = kernel.c
BINDIR = bin
OBJDIR = lib

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
	@mkdir -p $(BINDIR) $(OBJDIR)
	$(CC) -ffreestanding -fno-stack-protector -fno-stack-check -D__$(ARCH)__ -I. -c $< -o $(OBJDIR)/kernel.o
	$(LD) -nostdlib -z max-page-size=0x1000 -Ttext=0x01000000 $(OBJDIR)/kernel.o -o $(BINDIR)/$(TARGET)

clean:
	rm -rf $(OUTDIR)
	rm -rf $(OBJDIR)
