RULES = all clean nuke run fresh

.PHONY: $(RULES)

$(RULES):
	$(MAKE) -C src $@
