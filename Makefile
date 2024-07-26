RULES = all clean distclean run fresh

.PHONY: $(RULES)

$(RULES):
	$(MAKE) -C src $@
