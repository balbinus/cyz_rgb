#include <avr/interrupt.h>
#include "usiTwi/usiTwiSlave.h"
#include "cyz/cyz_rgb.h"
#include "cyz/cyz_cmd.h"
unsigned char cyz_slaveAddress;


Cyz_rgb* cyz_rgb;
Cyz_cmd* cyz_cmd;

int main(void)
{
	CYZ_RGB_GET_INSTANCE();
	CYZ_CMD_GET_INSTANCE();

	_CYZ_RGB_set_color(255,0,0);
	CYZ_CMD_load_boot_params();

	cyz_slaveAddress = 0x26;		// This can be changed to your own address
	usiTwiSlaveInit(cyz_slaveAddress);

	TIFR   = (1 << TOV0);  /* clear interrupt flag */
	TIMSK  = (1 << TOIE0); /* enable overflow interrupt */
	TCCR0B = (1 << CS00);  /* start timer, no prescale */
	sei(); // enable interrupts

	for(;;)
	{
		while(usiTwiDataInReceiveBuffer()) {
			_CYZ_CMD_receive_one_byte(usiTwiReceiveByte());
		}
	}

	return 1;
}


/*	Triggered when timer overflows. */
/*  This runs fast enough that 255 calls are less than a glimpse for a human. */
/*  TODO: figure out _actual_ math */
ISR(SIG_OVERFLOW0)
{
	static unsigned int sigcount = -1;
	if (++sigcount == 0) { //TODO: better to use another clock, prescaled
		_CYZ_CMD_play_next_script_line();
	}

	_CYZ_RGB_pulse();
}



