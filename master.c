#include <avr/interrupt.h>
#include "cyz/cyz_cmd.h"
#include "cyz/cyz_rgb.h"

#define TWI_GEN_CALL         0x00
#include "usiTwi/usiTwiMaster.h"

int main(void) {
	CYZ_RGB_GET_INSTANCE();
	CYZ_CMD_GET_INSTANCE();

	cyz_rgb.color.r = 255;


	/*
		line1.dur = 255;
		line1.cmd[0] = 'c';
		line1.cmd[1] = 255;
		line1.cmd[2] = 0;
		line1.cmd[3] = 0;
	 */
	/*
	{
		int i;
		unsigned char line1[8] = { 'W', 0, 0, 255, 'c', 0, 0, 255 };

		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 1;
		line1[3] = 255;
		line1[4] = 'h';
		line1[5] = 0;
		line1[6] = 100;
		line1[7] = 0;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 2;
		line1[3] = 255;
		line1[4] = 'c';
		line1[5] = 255;
		line1[6] = 0;
		line1[7] = 0;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 3;
		line1[3] = 255;
		line1[4] = 'c';
		line1[5] = 255;
		line1[6] = 255;
		line1[7] = 255;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 4;
		line1[3] = 50;
		line1[4] = 'c';
		line1[5] = 0;
		line1[6] = 0;
		line1[7] = 0;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 5;
		line1[3] = 50;
		line1[4] = 'c';
		line1[5] = 200;
		line1[6] = 0;
		line1[7] = 100;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 6;
		line1[3] = 50;
		line1[4] = 'c';
		line1[5] = 100;
		line1[6] = 0;
		line1[7] = 200;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 7;
		line1[3] = 50;
		line1[4] = 'c';
		line1[5] = 200;
		line1[6] = 200;
		line1[7] = 0;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 8;
		line1[3] = 50;
		line1[4] = 'c';
		line1[5] = 0;
		line1[6] = 200;
		line1[7] = 200;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}

		line1[0] = 'W';
		line1[1] = 0;
		line1[2] = 9;
		line1[3] = 50;
		line1[4] = 'c';
		line1[5] = 100;
		line1[6] = 200;
		line1[7] = 0;
		for(i=0;i<8;i++) {
			_CYZ_CMD_receive_one_byte(line1[i]);
		}


		unsigned char lenr[4];
		lenr[0] = 'L';
		lenr[1] = 0;
		lenr[2] = 2;
		lenr[3] = 2;
		for (i=0;i<4;i++) {
			_CYZ_CMD_receive_one_byte(lenr[i]);
		}
		CYZ_CMD_load_boot_params();
		unsigned char bootdo[6];
		bootdo[0] = 'B';
		bootdo[1] = 1;
		bootdo[2] = 0;
		bootdo[3] = 0;
		bootdo[4] = 150;
		bootdo[5] = 150;
		for (i=0;i<6;i++) {
			_CYZ_CMD_receive_one_byte(bootdo[i]);
		}

		CYZ_CMD_load_boot_params();
	}
*/

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
	messageBuf[2] = cyz_rgb.fade_color.r;
	messageBuf[3] = cyz_rgb.fade_color.g;
	messageBuf[4] = cyz_rgb.fade_color.b;
	unsigned char success = USI_TWI_Start_Transceiver_With_Data( messageBuf, 5 );
	if (!success) {
		USI_TWI_Master_Initialise();

		/*
		switch(USI_TWI_Get_State_Info( )) {

		case USI_TWI_NO_DATA:             // Transmission buffer is empty
			//CYZ_RGB_set_color(255, 0 , 0);
			break;
		case USI_TWI_DATA_OUT_OF_BOUND:   // Transmission buffer is outside SRAM space
			//CYZ_RGB_set_color(0, 255 , 0);
			break;
		case USI_TWI_UE_START_CON:        // Unexpected Start Condition
			//CYZ_RGB_set_color(255, 0 , 0);
			break;
		case USI_TWI_UE_STOP_CON:        // Unexpected Stop Condition
			//CYZ_RGB_set_color(255, 255 , 0);
			break;
		case USI_TWI_UE_DATA_COL:         // Unexpected Data Collision (arbitration)
			//CYZ_RGB_set_color(0, 255 , 255);
			break;
		case USI_TWI_NO_ACK_ON_DATA:      // The slave did not acknowledge  all data
			//CYZ_RGB_set_color(255, 0 , 255);
			break;
		case USI_TWI_NO_ACK_ON_ADDRESS:   // The slave did not acknowledge  the address
			//CYZ_RGB_set_color(50, 0 , 0);
			break;
		case USI_TWI_MISSING_START_CON:   // Generated Start Condition not detected on bus
			//CYZ_RGB_set_color(0, 50 , 0);
			break;
		case USI_TWI_MISSING_STOP_CON:    // Generated Stop Condition not detected on bus
			//CYZ_RGB_set_color(0, 0 , 50);
			break;
		}
		*/
	}
}

/*	Triggered when timer overflows. */
ISR(SIG_OVERFLOW0)
{
	_CYZ_CMD_tick();
	cyz_master_send_color();
	_CYZ_RGB_pulse();
}
