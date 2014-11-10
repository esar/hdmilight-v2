BIN_FILES = $(shell awk '{print $$2}' merge.conf)


all: merged.bin

output_%.bin: output_%.conf
	../../tools/output2bin.py $< > $@

area.bin: area.conf
	../../tools/area2bin.py $< > $@

gamma_%.bin: gamma_%.conf
	../../tools/gamma2bin.py $(shell cat $<) > $@

colour_%.bin: colour_%.conf
	../../tools/colour2bin.py $< > $@

delay.bin: delay.conf
	../../tools/delay2bin.py $< > $@

merged.bin: merge.conf $(BIN_FILES)
	../../tools/mergebin.py merge.conf merged.bin

clean:
	rm *.bin
