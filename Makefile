# Make Makefile in src/ directory

.PHONY: all run rerun clean

export NOT_IN_DOCKER := 1

all: src/
	$(MAKE) -C $<

run: src/
	$(MAKE) -C $< run

clean:
	$(MAKE) -C src/ clean

rerun: clean run
