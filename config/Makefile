CONFIGS = $(shell awk '{print $$2}' merge.conf)

all: merged.bin

.PHONY: $(CONFIGS)

$(CONFIGS):
	make -C $@

merged.bin: merge.conf $(CONFIGS)
	../tools/mergebin.py merge.conf merged.bin

clean:
	@$(foreach CONFIG, $(CONFIGS), $(MAKE) -C $(CONFIG) clean;)
	rm *.bin
