# Make Makefile in src/ directory

.PHONY: all run rerun clean

all: src/
	$(MAKE) -C $<

run: src/
	$(MAKE) -C $< run

clean:
	$(MAKE) -C src/ clean

rerun: clean run
