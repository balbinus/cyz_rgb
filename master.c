#include <avr/interrupt.h>
#include "cyz/cyz_rgb.h"

#define TWI_GEN_CALL         0x00
#include "usiTwi/usiTwiMaster.h"

CYZ_RGB_setup();

int main(void)
{
	CYZ_RGB_init();

	USI_TWI_Master_Initialise();

	TIFR   = (1 << TOV0);  /* clear interrupt flag */
  	TIMSK  = (1 << TOIE0); /* enable overflow interrupt */
	TCCR0B = (1 << CS00);  /* start timer, no prescale */

	sei(); // enable interrupts

	for(;;)
	{}

	return 1;
}

void cyz_master_send_color() {
	unsigned char messageBuf[5];
	// send new color to slave
	messageBuf[0] = TWI_GEN_CALL; // The first byte must always consist of General Call code or the TWI slave address.
    messageBuf[1] = 'c';          // The command or data to be included in the general call.
    messageBuf[2] = CYZ_RGB_fade_color_r;
    messageBuf[3] = CYZ_RGB_fade_color_g;
    messageBuf[4] = CYZ_RGB_fade_color_b;
    unsigned char success = USI_TWI_Start_Transceiver_With_Data( messageBuf, 5 );
    if (!success) {
		USI_TWI_Master_Initialise();

		switch(USI_TWI_Get_State_Info( )) {

			case USI_TWI_NO_DATA:             // Transmission buffer is empty
				CYZ_RGB_set_color(255, 0 , 0);
			break;
			case USI_TWI_DATA_OUT_OF_BOUND:   // Transmission buffer is outside SRAM space
				CYZ_RGB_set_color(0, 255 , 0);
			break;
			case USI_TWI_UE_START_CON:        // Unexpected Start Condition
				CYZ_RGB_set_color(255, 0 , 0);
			break;
			case USI_TWI_UE_STOP_CON:        // Unexpected Stop Condition
				CYZ_RGB_set_color(255, 255 , 0);
			break;
			case USI_TWI_UE_DATA_COL:         // Unexpected Data Collision (arbitration)
				CYZ_RGB_set_color(0, 255 , 255);
			break;
			case USI_TWI_NO_ACK_ON_DATA:      // The slave did not acknowledge  all data
				CYZ_RGB_set_color(255, 0 , 255);
			break;
			case USI_TWI_NO_ACK_ON_ADDRESS:   // The slave did not acknowledge  the address
				CYZ_RGB_set_color(50, 0 , 0);
			break;
			case USI_TWI_MISSING_START_CON:   // Generated Start Condition not detected on bus
				CYZ_RGB_set_color(0, 50 , 0);
			break;
			case USI_TWI_MISSING_STOP_CON:    // Generated Stop Condition not detected on bus
				CYZ_RGB_set_color(0, 0 , 50);
			break;
		}
	}
}

/*	Triggered when timer overflows. */

ISR(SIG_OVERFLOW0)
{
	static unsigned int sigcount = -1;
	if (++sigcount == 0) { //TODO: better to use another clock, prescaled
		CYZ_RGB_fade_color_r += 100;
		CYZ_RGB_fade_color_g += 70;
		CYZ_RGB_fade_color_b += 50;
		// TODO: learn to predict how long beteen each overflow
		cyz_master_send_color();
	}

	CYZ_RGB_pulse();
}
