RM := rm -rf

# All Target
all:  clean out/master.hex out/slave.hex

#cyz_cmd
out/cyz_cmd.o: out/cyz_rgb.o
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=attiny45 -DF_CPU=1000000UL -MMD -MP -MF"out/cyz_cmd.d" -MT"out/cyz_cmd.d" -c -o"out/cyz_cmd.o" "../cyz/cyz_cmd.c"
	@echo 'Finished building: $<'
	@echo ' '
	
#cyz_rgb
out/cyz_rgb.o:
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=attiny45 -DF_CPU=1000000UL -MMD -MP -MF"out/cyz_rgb.d" -MT"out/cyz_rgb.d" -c -o"out/cyz_rgb.o" "../cyz/cyz_cmd.c"
	@echo 'Finished building: $<'
	@echo ' '
	
# Master-mode Firmware
out/master.hex: out/master.elf sizedummy-master
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex out/master.elf  "out/master.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy-master: out/master.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny45 out/master.elf
	@echo 'Finished building: $@'
	@echo ' '

out/master.elf: out/master.o out/usiTwiMaster.o out/cyz_cmd.o out/cyz_rgb.o
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,out/master.map -L/opt/local/avr/include -mmcu=attiny45 -o"out/master.elf"  out/master.o  out/usiTwiMaster.o  out/cyz_cmd.o out/cyz_rgb.o
	@echo 'Finished building target: $@'
	@echo ' '

out/master.o:
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=attiny45 -DF_CPU=1000000UL -MMD -MP -MF"out/master.d" -MT"out/master.d" -c -o"out/master.o" "../master.c"
	@echo 'Finished building: $<'
	@echo ' '
	
out/usiTwiMaster.o:
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=attiny45 -DF_CPU=1000000UL -MMD -MP -MF"out/usiTwiMaster.d" -MT"out/usiTwiMaster.d" -c -o"out/usiTwiMaster.o" "../usiTwi/usiTwiMaster.c"
	@echo 'Finished building: $<'
	@echo ' '

# Slave-mode firmware
out/slave.hex: out/slave.elf sizedummy-slave
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex out/slave.elf  "out/slave.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy-slave: out/slave.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny45 out/slave.elf
	@echo 'Finished building: $@'
	@echo ' '

out/slave.elf: out/slave.o out/usiTwiSlave.o
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,out/slave.map -L/opt/local/avr/include -mmcu=attiny45 -o"out/slave.elf"  out/slave.o  out/usiTwiSlave.o   
	@echo 'Finished building target: $@'
	@echo ' '

out/slave.o:
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=attiny45 -DF_CPU=1000000UL -MMD -MP -MF"out/slave.d" -MT"out/slave.d" -c -o"out/slave.o" "../slave.c"
	@echo 'Finished building: $<'
	@echo ' '
	
out/usiTwiSlave.o:
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=attiny45 -DF_CPU=1000000UL -MMD -MP -MF"out/usiTwiSlave.d" -MT"out/usiTwislave.d" -c -o"out/usiTwiSlave.o" "../usiTwi/usiTwiSlave.c"
	@echo 'Finished building: $<'
	@echo ' '

# Other Targets
clean:
	-$(RM) out/*
	-@echo ' '

#secondary-outputs: out/master.hex out/sizedummy

.PHONY: all clean dependents
.SECONDARY:
