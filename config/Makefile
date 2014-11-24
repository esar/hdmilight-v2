CONFIGS = $(shell awk '!/\.bin/{print $$2}' merge.conf)

all: merged.bin

.PHONY: $(CONFIGS)

$(CONFIGS):
	make -C $@

format.bin: format.conf
	../tools/format2bin.py $< > $@

merged.bin: merge.conf format.bin $(CONFIGS)
	../tools/mergebin.py merge.conf merged.bin

clean:
	@$(foreach CONFIG, $(CONFIGS), $(MAKE) -C $(CONFIG) clean;)
	rm -f *.bin
