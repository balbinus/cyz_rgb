#include <avr/interrupt.h>
#include "usiTwi/usiTwiSlave.h"
#include "cyz/cyz_rgb.h"
#include "cyz/cyz_cmd.h"
unsigned char cyz_slaveAddress;

//CYZ_RGB_setup();

Cyz_rgb* cyz_rgb;

int main(void)
{
	Cyz_rgb* cyz_rgb = CYZ_RGB_GET_INSTANCE();

	cyz_rgb->init();
	cyz_rgb->set_color(cyz_rgb, 0,0,0);
	//Cyz_rgb->init();
	//CYZ_RGB_init();

	cyz_slaveAddress = 0x26;		// This can be change to your own address
	usiTwiSlaveInit(cyz_slaveAddress);
	TIFR   = (1 << TOV0);  /* clear interrupt flag */
  	TIMSK  = (1 << TOIE0); /* enable overflow interrupt */
	TCCR0B = (1 << CS00);  /* start timer, no prescale */

	sei(); // enable interrupts


	//CYZ_RGB_set_color(127,255,0);

	for(;;)
	{
		CYZ_CMD_receive_and_execute(cyz_rgb);
	}

	return 1;
}


/*	Triggered when timer overflows. */
ISR(SIG_OVERFLOW0)
{
	cyz_rgb->pulse(cyz_rgb);
}



