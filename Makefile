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
AVR_LINK = avr-gcc 
AVR_SIZE = avr-size --format=avr --mcu=$(MCU)

CC = avr-gcc
CFLAGS = -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=1000000UL -MMD -MP -std=gnu99
LD = avr-gcc
LDFLAGS = -mmcu=$(MCU)

OBJCOPY = avr-objcopy
TEST_COMPILER = gcc -D TEST_MODE -Wall -Os -ftest-coverage -fprofile-arcs -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields

TARGETS = out/master.hex out/slave.hex
BASE_OBJS = cyz/cyz_rgb.o cyz/cyz_cmd.o cyz/color.o 
MASTER_OBJS = master.o usiTwi/usiTwiMaster.o
SLAVE_OBJS = slave.o usiTwi/usiTwiSlave.o
DEPS = $(MASTER_OBJS:.o=.d) $(SLAVE_OBJS:.o=.d)

# All Target
all: $(TARGETS) sizes

%.hex: %.elf
	$(OBJCOPY) $< $@

out/master.elf: $(BASE_OBJS) $(MASTER_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

out/slave.elf: $(BASE_OBJS) $(SLAVE_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

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
