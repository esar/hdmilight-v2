MAX_IMAGE_SIZE = 24576

PRG            = image
OBJ            = main.o serial.o timer.o i2c.o \
                 fixed_point.o \
                 stackmon.o    \
                 cmd_addr.o    \
                 cmd_colour.o  \
                 cmd_delay.o   \
                 cmd_gamma.o   \
                 cmd_i2c.o     \
                 cmd_area.o    \
                 cmd_output.o  \
                 cmd_port.o    \
                 cmd_result.o  \
                 cmd_status.o  \
                 cmd_format.o  \
                 cmd_keys.o    \
                 cmd_mem.o     \
                 cmd_stack.o

MCU_TARGET     = atmega16
OPTIMIZE       = -Os

DEFS           = 
LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc

CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -Wl,--defsym=__stack=0x8007ff
LDFLAGS       = -Wl,-Map,$(PRG).map

# Strip unused code
CFLAGS += -ffunction-sections -fno-unroll-loops -fno-inline -fno-jump-tables
LDFLAGS += -Wl,-gc-sections  

#LDFLAGS += --cref -nostartfiles

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

MAKEMEM = ../tools/makemem

all: $(PRG).elf lst text mem

main.o: config_edid.h config_hdmi.h

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf
	rm -rf *.lst *.map *.bin *.lss *.sym *.hex $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -d $< > $@

# Rules for building the .text rom images

text: bin

hex:  $(PRG).hex
bin:  $(PRG).bin
mem:  $(PRG).mem
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
	
%.mem: %.bin
	$(MAKEMEM) $< $(MAX_IMAGE_SIZE) > $@

