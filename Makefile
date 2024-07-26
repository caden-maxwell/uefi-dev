# Make Makefile in src/ directory

.PHONY: all run clean fresh

all: src/
	$(MAKE) -C $<

run: src/
	$(MAKE) -C $< run

clean:
	$(MAKE) -C src/ clean

fresh: clean all
