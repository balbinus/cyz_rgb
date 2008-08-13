#include <avr/interrupt.h>
#include "usiTwi/usiTwiSlave.h"
#include "cyz/cyz_rgb.h"
#include "cyz/cyz_cmd.h"
unsigned char cyz_slaveAddress;


Cyz_rgb* cyz_rgb;
Cyz_cmd* cyz_cmd;

int main(void)
{
	cyz_rgb = CYZ_RGB_GET_INSTANCE();
	cyz_rgb->init();
	cyz_rgb->set_color(cyz_rgb, 255,125,50);

	cyz_cmd = CYZ_CMD_GET_INSTANCE(cyz_rgb);

	cyz_slaveAddress = 0x26;		// This can be changed to your own address
	usiTwiSlaveInit(cyz_slaveAddress);

	TIFR   = (1 << TOV0);  /* clear interrupt flag */
  	TIMSK  = (1 << TOIE0); /* enable overflow interrupt */
	TCCR0B = (1 << CS00);  /* start timer, no prescale */
	sei(); // enable interrupts

	for(;;)
	{
		if(usiTwiDataInReceiveBuffer()) {
			cyz_cmd->receive_one_byte(cyz_cmd, usiTwiReceiveByte());
		}
	}

	return 1;
}


/*	Triggered when timer overflows. */
/*  This runs fast enough that 255 calls are less than a glimpse for a human. */
/*  TODO: figure out _actual_ math */
ISR(SIG_OVERFLOW0)
{
	cyz_rgb->pulse(cyz_rgb);
}



