###################################################################
# Specify thetarget MCU for compilation on the commandline.
###################################################################
#
# For BlinkM use attiny45:
#
#  make MCU=attiny45
#
# For BlinkM MaxM use attiny44:
#
#  make MCU=attiny44
#
MCU := attiny44
RM := rm -rf
AVR_LINK = avr-gcc -Wl,-Map,out/slave.map -L/opt/local/avr/include -mmcu=$(MCU)
AVR_SIZE = avr-size --format=avr --mcu=$(MCU)
AVR_COMPILE_LOG = 'AVR Compiler: $@.c --> $@.o'

CC = avr-gcc
CFLAGS = -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=1000000UL -MMD -MP -c
#~ AVR_COMPILE = avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=1000000UL -MMD -MP \
	#~ -MF"out/$@.d" -MT"out/$@.d" -c -o"out/$@.o" 

AVR_FLASH = avr-objcopy -R .eeprom -O ihex
TEST_COMPILER = gcc -D TEST_MODE -Wall -Os -ftest-coverage -fprofile-arcs -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields

TARGETS = out/master.hex out/slave.hex
BASE_OBJS = cyz/cyz_rgb.o cyz/cyz_cmd.o cyz/color.o 
MASTER_OBJS = master.o usiTwi/usiTwiMaster.o
SLAVE_OBJS = slave.o usiTwi/usiTwiSlave.o
DEPS = $(MASTER_OBJS:.o=.d) $(SLAVE_OBJS:.o=.d)

# All Target
all: $(TARGETS) sizes

%.hex: %.elf
	$(AVR_FLASH) $< $@

out/master.elf: $(BASE_OBJS) $(MASTER_OBJS)
	$(AVR_LINK) -o $@ $^

out/slave.elf: $(BASE_OBJS) $(SLAVE_OBJS)
	$(AVR_LINK) -o $@ $^

#print sizes of binary	
sizes: out/slave.elf out/master.elf
	$(AVR_SIZE) out/slave.elf
	$(AVR_SIZE) out/master.elf

clean:
	-$(RM) *.o **/*.o *.d **/*.d out/*
	-@echo ' '
	
.PHONY: all clean dependents
.SECONDARY:

# added "-" in the beginning, so that we don't get an error if the file is not present
-include $(DEPS)
