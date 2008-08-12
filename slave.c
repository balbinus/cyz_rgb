#include <avr/interrupt.h>
#include "cyz/cyz_rgb.h"

#include "usiTwi/usiTwiSlave.h"
unsigned char cyz_slaveAddress;

CYZ_RGB_setup();

int main(void)
{
	CYZ_RGB_init();

	cyz_slaveAddress = 0x26;		// This can be change to your own address
	usiTwiSlaveInit(cyz_slaveAddress);
	TIFR   = (1 << TOV0);  /* clear interrupt flag */
  	TIMSK  = (1 << TOIE0); /* enable overflow interrupt */
	TCCR0B = (1 << CS00);  /* start timer, no prescale */

	sei(); // enable interrupts


	CYZ_RGB_set_color(127,255,0);
	int cnt = 0;
	char rcv[4];
	for(;;)
	{
	    if(usiTwiDataInReceiveBuffer())
	    {
	    	rcv[cnt++] = usiTwiReceiveByte();
			if (cnt==4) {
				switch (rcv[0]) {
				case 'n':
					CYZ_RGB_set_color(rcv[1], rcv[2] , rcv[3]);
				break;
				case 'c':
					CYZ_RGB_set_fade_color(rcv[1],rcv[2],rcv[3]);
				break;
				}
				cnt = 0;
			}
	    }
	}

	return 1;
}


/*	Triggered when timer overflows. */

ISR(SIG_OVERFLOW0)
{
	CYZ_RGB_pulse();
}



