#include <avr/interrupt.h>
#include "usiTwi/usiTwiSlave.h"
#include "cyz/cyz_rgb.h"
#include "ring_buffer.h"
#include "cyz/cyz_cmd.h"

#ifndef TIMSK
#  define TIMSK TIMSK0
#endif
#ifndef TIFR
#  define TIFR TIFR0
#endif

Color led_curr_color;
Color led_fade_color;
uint8_t led_fade;
uint8_t led_fadespeed;

int main(void)
{
	CYZ_RGB_init();
	CYZ_CMD_init();

	led_curr_color.r = 0;
	led_curr_color.g = 0;
	led_curr_color.b = 0;
	
	CYZ_CMD_load_boot_params();
	usiTwiSlaveInit();

	TIFR   = (1 << TOV0);  /* clear interrupt flag */
	TIMSK  = (1 << TOIE0); /* enable overflow interrupt */
	TCCR0B = (1 << CS00);  /* start timer, no prescale */
	sei(); // enable interrupts

	for(;;)
	{
		while(usiTwiDataInReceiveBuffer()) {
			CYZ_CMD_receive_one_byte(usiTwiReceiveByte());
		}

		while( ring_buffer_has_data(&cyz_cmd.send_buffer)) {
			usiTwiTransmitByte(ring_buffer_pop(&cyz_cmd.send_buffer));
		}
	}

	return 1;
}

/*	Triggered when timer overflows. */
/*  This runs fast enough that 255 calls are less than a glimpse for a human. */
/*  TODO: figure out _actual_ math */
ISR(SIG_OVERFLOW0)
{
	CYZ_CMD_tick();
	_CYZ_RGB_pulse();
}
