#include <avr/interrupt.h>

#include "usiTwi/usiTwiSlave.h"
#include "cyz/cyz_rgb.h"
#include "ring_buffer.h"
#include "cyz/cyz_cmd.h"
#include "pwm_logtable.h"

Color led_curr_color;	//Current LED color
Color led_fade_color;	//Target color after fade
uint8_t led_fade;		//LED is fading TRUE/FALSE
uint8_t led_fadespeed;	//Speed of fade

/**
 * Pinout on ATTiny44 (BlinkM MaxM):
 * 
 *                (+) VCC  1 -     - 14 GND (-)
 *                 NC/PB0  2 -     - 13 PA0/ADC0
 *                 NC/PB1  3 -     - 12 PA1/ADC1
 *             !RESET/PB3  4 -     - 11 PA2/ADC2
 *                RED/PB2  5 -     - 10 PA3/ADC3
 *              GREEN/PA7  6 -     - 9  PA4/SCL
 *                SDA/PA6  7 -     - 8  PA5/BLUE
 */

int main(void)
{
	CYZ_RGB_init();
	CYZ_CMD_init();

	// Set some initial values if we are debugging
	//led_curr_color.r = 40;
	//led_curr_color.g = 200;
	//led_curr_color.b = 128;

	CYZ_CMD_load_boot_params();
	usiTwiSlaveInit();

#if defined( BLINKM_TYPE_BLINKM )
	OCR0A  = 0xFF;		//Timer0 (Red pin) Output Compare
	OCR0B  = 0xFF;		//Timer0 (Green pin) Output Compare

	TIFR   =  (1<< TOV0) 							//Timer0 clear interrupts
			 |(1<< TOV1);							//Timer1 clear interrupts
    TIMSK  =  (1<< TOIE0);							//Timer0 enable overflow ISR
    TCCR0B =  (1<< CS00);							//Timer0 Start, no prescale
	TCCR1  =  (1<< CS10);							//Timer1 Start, no prescale
	
#endif

#if defined( BLINKM_TYPE_MAXM )
	ICR1   = 0xFFFF;	//Timer1 "TOP"
	OCR1B  = 0xFFFF;	//Timer1 (Blue pin) Output Compare
	OCR0A  = 0xFF;		//Timer0 (Red pin) Output Compare
	OCR0B  = 0xFF;		//Timer0 (Green pin) Output Compare

	TIFR1  =  (1<< TOV0);								//Timer1 clear interrupts
	TIFR0  =  (1<< TOV0);								//Timer0 clear interrupts
	TIMSK1 =  (1<< TOIE0);								//Timer1 enable overflow ISR
	TIMSK0 =  (1<< TOIE0);								//Timer0 enable overflow ISR
	TCCR1A =  (1<< COM1B1) |(1<< COM1B0)				//Timer1 Inverted PWM (Blue)
	         |(1<< WGM11); 								//Timer1 16 bit fast PWM
	TCCR1B =  (1<< WGM12)  |(1<< WGM13)					//Timer1 16 bit fast PWM
			 |(1<< CS00);								//Timer1 Start, no prescale
	TCCR0B =  (1<< CS00); 								//Timer0 Start, no prescale
#endif

    sei();												//Enable Interrupts

	for(;;) {
		while(usiTwiDataInReceiveBuffer()) {
			CYZ_CMD_receive_one_byte(usiTwiReceiveByte());
		}

		while( ring_buffer_has_data(&cyz_cmd.send_buffer)) {
			usiTwiTransmitByte(ring_buffer_pop(&cyz_cmd.send_buffer));
		}
	}

	return 1;
}

#if defined ( BLINKM_TYPE_MAXM )
/*  ISR Triggered at the end of the PWM period when 16 bit timer overflows
 *  This controls the blue LED PWM and calls other housekeeping tasks.
 *
 *  On the BlinkM MaxM we use the 16 bit timer for hardware PWM on the
 *  Blue channel, and we also put our fade routines etc in this ISR.
 *  We use inverse PWM because our software PWM on the other two channels
 *  is inverse modulated.
 */
ISR(SIG_OVERFLOW1) {

	static uint16_t pwm_period_blu;

	CYZ_CMD_tick();

	if(led_fade) {
		_CYZ_RGB_fade_step_component(&led_curr_color.r, &led_fade_color.r);
		_CYZ_RGB_fade_step_component(&led_curr_color.g, &led_fade_color.g);
		_CYZ_RGB_fade_step_component(&led_curr_color.b, &led_fade_color.b);
	}

	pwm_period_blu = PWM_LOG(led_curr_color.b);
 
	OCR1B = pwm_period_blu;
}
#endif

/* 8 bit timer overflow ISR - used to simulate 16 bit timers
 * This controls the red and green LEDs on MaxM and all LEDs on BlinkM.
 *
 * Theory of operation:
 *
 * Timer0 has 2 8 bit compare registers that can trigger an ISR. We can
 * simulate 16 bit PWM by manipulating these compare registers on each
 * overflow of Timer0. If we are careful, we can achieve reasonable
 * flicker-free 16 bit PWM.
 * 
 * The PWM period lasts for 65536 cycles or 256 overflows of Timer0. We keep
 * our low byte in SRAM (timer0_tick). We take the 16 bit PWM period for each
 * of our outputs and then calculate an overflow count and a modulus. Since
 * we are using inverse PWM, we start each PWM cycle with the LEDs off and
 * then when the appropriate number of timer0 overflows have elapsed, we set
 * a match counter for the particular color that will trigger a small ISR at
 * the appropriate cycle to power the LED on.
 *
 * We use inverted PWM because at the beginning of a PWM cycle, this ISR
 * takes a lot of cycles to run. Since modulating the LEDs to very dim values
 * requires them being lit for only a few cycles, without inverse PWM, we'd
 * end up having to switch them off before we even leave our ISR. With
 * inverse PWM, the duty cycle is not 100% even when the LED's are at maximum
 * brightness; however, there is no visual brightness difference.
 *
 */
ISR(SIG_OVERFLOW0) {

	static uint8_t timer0_tick;

	static uint8_t count_grn;
	static uint8_t modulus_grn;
	static uint16_t pwm_period_grn;

	static uint8_t count_red;
	static uint8_t modulus_red;
	static uint16_t pwm_period_red;	
	
#if defined( BLINKM_TYPE_BLINKM )
	static uint8_t count_blu;
	static uint8_t modulus_blu;
	static uint16_t pwm_period_blu;
#endif

	//Check if we are beginning of a new PWM period (16-bit overflow)
	if (timer0_tick == 0) {

		//Turn LEDs off
		RED_LED_OFF;
		GRN_LED_OFF;
#if defined( BLINKM_TYPE_BLINKM )
		BLU_LED_OFF;

		CYZ_CMD_tick();

		if(led_fade) {
			_CYZ_RGB_fade_step_component(&led_curr_color.r, &led_fade_color.r);
			_CYZ_RGB_fade_step_component(&led_curr_color.g, &led_fade_color.g);
			_CYZ_RGB_fade_step_component(&led_curr_color.b, &led_fade_color.b);
		}

		pwm_period_blu = PWM_LOG(led_curr_color.b);
		count_blu   = pwm_period_blu / 256;
		modulus_blu = pwm_period_blu % 256;
#endif

		pwm_period_red = PWM_LOG(led_curr_color.r);
		count_red   = pwm_period_red / 256;
		modulus_red = pwm_period_red % 256;

		pwm_period_grn = PWM_LOG(led_curr_color.g);
		count_grn   = pwm_period_grn / 256;
		modulus_grn = pwm_period_grn % 256;

    }

#if defined( BLINKM_TYPE_BLINKM )
    if (timer0_tick == count_red && led_curr_color.r) {
		OCR0A = modulus_red;
		TIMSK |=  (1<< OCIE0A);
    }
    if (timer0_tick == count_grn && led_curr_color.g) {
		OCR0B = modulus_grn;
		TIMSK |= (1<< OCIE0B);
    }
	if (timer0_tick == count_blu && led_curr_color.b) {
		OCR1A = modulus_blu;
		TIMSK |=  (1<< OCIE1A);
    }
#endif
#if defined( BLINKM_TYPE_MAXM )
    if (timer0_tick == count_red && led_curr_color.r) {
		OCR0A = modulus_red;
		TIMSK0 |=  (1<< OCIE0A);
    }
    if (timer0_tick == count_grn && led_curr_color.g) {
		OCR0B = modulus_grn;
		TIMSK0 |= (1<< OCIE0B);
    }
#endif

	timer0_tick++;

}

#if defined( BLINKM_TYPE_BLINKM )
ISR(SIG_OUTPUT_COMPARE0A) {
	TIMSK &= ~(1<< OCIE0A);
	RED_LED_ON;
}
ISR(SIG_OUTPUT_COMPARE0B) {
	TIMSK &= ~(1<< OCIE0B);
	GRN_LED_ON;
}
ISR(SIG_OUTPUT_COMPARE1A) {
	TIMSK &= ~(1<< OCIE1A);
	BLU_LED_ON;
}
#endif
#if defined( BLINKM_TYPE_MAXM )
ISR(SIG_OUTPUT_COMPARE0A) {
	TIMSK0 &= ~(1<< OCIE0A);
	RED_LED_ON;
}
ISR(SIG_OUTPUT_COMPARE0B) {
	TIMSK0 &= ~(1<< OCIE0B);
	GRN_LED_ON;
}
#endif
