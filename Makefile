# Make Makefile in src/ directory

.PHONY: all run fresh clean

all: src/ kernel
	$(MAKE) -C $<

kernel: src/
	$(MAKE) -C $< kernel

run: src/
	$(MAKE) -C $< run

clean:
	$(MAKE) -C src/ clean

fresh: clean all
