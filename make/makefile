RM := rm -rf
ECHO = @
AVR_LINK = $(ECHO) avr-gcc -Wl,-Map,out/slave.map -L/opt/local/avr/include -mmcu=attiny45
AVR_SIZE = $(ECHO) -avr-size --format=Berkeley
AVR_COMPILE_LOG = 'AVR Compiler: $@.c --> $@.o'
AVR_COMPILE = $(ECHO)avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=attiny45 -DF_CPU=1000000UL -MMD -MP \
-MF"out/$@.d" -MT"out/$@.d" -c -o"out/$@.o" 
AVR_FLASH = $(ECHO)-avr-objcopy -R .eeprom -O ihex
TEST_COMPILER = $(ECHO)gcc -D TEST_MODE -Wall -Os -ftest-coverage -fprofile-arcs -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields

# All Target
all:  clean out/master.hex out/slave.hex sizes test/test

#objects
cyz_cmd:
	@echo $(AVR_COMPILE_LOG) 
	$(AVR_COMPILE) ../cyz/cyz_cmd.c

cyz_rgb:
	@echo $(AVR_COMPILE_LOG) 
	$(AVR_COMPILE) ../cyz/cyz_rgb.c

color:
	@echo $(AVR_COMPILE_LOG) 
	$(AVR_COMPILE) ../cyz/color.c
	
usiTwiMaster:
	@echo $(AVR_COMPILE_LOG) 
	$(AVR_COMPILE) ../usiTwi/usiTwiMaster.c

usiTwiSlave:
	@echo $(AVR_COMPILE_LOG) 
	$(AVR_COMPILE) ../usiTwi/usiTwiSlave.c

#master firmware
master:
	@echo $(AVR_COMPILE_LOG) 
	$(AVR_COMPILE) ../master.c

out/master.elf: master usiTwiMaster cyz_rgb cyz_cmd color
	@echo 'AVR Linker: master.elf'
	$(AVR_LINK) -o"out/master.elf"  out/master.o  out/usiTwiMaster.o out/cyz_cmd.o out/cyz_rgb.o out/color.o
	
out/master.hex: out/master.elf
	@echo 'AVR Flash: master.elf --> master.hex'
	$(AVR_FLASH) out/master.elf  "out/master.hex"

#slave firmware
slave:
	@echo $(AVR_COMPILE_LOG) 
	$(AVR_COMPILE) ../slave.c

out/slave.hex: out/slave.elf
	@echo 'AVR Flash: slave.elf --> slave.hex'
	$(AVR_FLASH) out/slave.elf  "out/slave.hex"

out/slave.elf: slave usiTwiSlave cyz_cmd cyz_rgb color
	@echo 'AVR Linker: slave.elf'
	$(AVR_LINK) -o"out/slave.elf"  out/slave.o  out/usiTwiSlave.o out/cyz_cmd.o out/cyz_rgb.o out/color.o 

#print sizes of binary	
sizes: out/slave.elf out/master.elf
	$(AVR_SIZE) out/slave.elf out/master.elf

clean:
	-$(RM) out/*
	-@echo ' '

#tests and coverage
test/test: 
	$(TEST_COMPILER) -MMD -MP -MF"out_test/color.d" -MT"out_test/color.d" -c -o "out_test/color.o" "../cyz/color.c"
	$(TEST_COMPILER) -MMD -MP -MF"out_test/cyz_cmd.d" -MT"out_test/cyz_cmd.d" -c -o "out_test/cyz_cmd.o" "../cyz/cyz_cmd.c"
	$(TEST_COMPILER) -o "out_test/test" "../test/test.c" out_test/cyz_cmd.o out_test/color.o
	@echo ''
	@out_test/test
	@echo `gcov  -a -b -c -n --object-directory=out_test/ ../cyz/cyz_cmd.c`
	
.PHONY: all clean dependents
.SECONDARY:
