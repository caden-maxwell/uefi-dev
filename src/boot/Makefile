ARCH = x86_64
TARGET = main.efi
SRC = $(TARGET:.efi=.c)
BINDIR = bin
OBJDIR = lib
UEFI = uefi

all: $(BINDIR)/$(TARGET)

# Clone ./uefi from POSIX-UEFI
$(UEFI):
	git clone -n https://gitlab.com/bztsrc/posix-uefi.git posix-uefi
	cd posix-uefi \
		&& git sparse-checkout set --no-cone '/$(UEFI)/*' \
		&& git checkout
	
	@# Move the inner directories to the root
	mv posix-uefi/$(UEFI) .
	rm -rf posix-uefi

$(BINDIR)/$(TARGET): $(SRC) $(UEFI)
	$(MAKE) -f $(UEFI)/Makefile TARGET=$(TARGET) ARCH=$(ARCH)
	mkdir -p $(BINDIR) $(OBJDIR)
	mv $(TARGET) $(BINDIR)
	mv $(TARGET:.efi=.o) $(OBJDIR)

clean distclean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)
ifneq ($(wildcard $(UEFI)),)
	ifeq ($@, distclean)
		rm -rf $(UEFI)
	else
		$(MAKE) -C $(UEFI) clean
	endif
else
	@echo "UEFI directory not found. Skipping..."
endif
