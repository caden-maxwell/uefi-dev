ARCH = x86_64
BINDIR = bin
OBJDIR = lib

TARGET = kernel.elf
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/$(TARGET)

ifeq ($(USE_GCC),)
CC = clang -target $(ARCH)-elf
LD = ld.lld
else
CC = $(ARCH)-elf-gcc
LD = $(ARCH)-elf-ld
endif

$(OBJDIR)/%.o: %.c
	@mkdir -p $(BINDIR) $(OBJDIR)
	$(CC) -ffreestanding -fno-stack-protector -fno-stack-check -D__$(ARCH)__ -Iinc -c $< -o $@

# UEFI wastes lots and lots of memory. Link our "kernel" at an address (32M) which isn't used by UEFI
$(BINDIR)/$(TARGET): $(OBJS)
	$(LD) -nostdlib -z max-page-size=0x1000 -Ttext=0x01000000 $(OBJS) -o $(BINDIR)/$(TARGET)

clean nuke:
	rm -rf $(BINDIR) $(OBJDIR)
