master.hex: every about 3 seconds chaneges color and broadcasts a fadeToColor command
slave.hex: this will listen for incoming fadeToRGB and goToRGB commands and behave accordingly

I'm currently flashing blinkms using an atmel AVRISP programmer with avrdude and this command line:
avrdude -pt45 -cavrispmkII -Pusb -b115200 -Uflash:w:make/out/master.hex:a 

The project is being developed with eclipse 3.4, avr plugin and avr-gcc on mac. Anyway, the makefile works well also from the console.