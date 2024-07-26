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

clean nuke:
	rm -rf $(BINDIR) $(OBJDIR)
ifneq ($(wildcard $(UEFI)),)
	@if [ $@ = "nuke" ]; then \
		action="rm -rf $(UEFI)"; \
	else \
	 	action="$(MAKE) -C $(UEFI) clean"; \
	fi; \
	echo $$action && $$action
else
	@echo "No $(UEFI) directory found"
endif
