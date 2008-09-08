#include "cyz_rgb.h"

extern Color led_curr_color;
extern Color led_fade_color;
extern uint8_t led_fade;
extern uint8_t led_fadespeed;



void __CYZ_RGB_fade_step() {
	if (led_curr_color.r != led_fade_color.r) {
		if (led_curr_color.r > led_fade_color.r) {
			uint8_t diff = led_curr_color.r - led_fade_color.r;
			led_curr_color.r -= MIN(diff, led_fadespeed);
		}
		else {
			uint8_t diff = led_fade_color.r - led_curr_color.r;
			led_curr_color.r += MIN(diff, led_fadespeed);
		}
	}
	if (led_curr_color.g != led_fade_color.g) {
		if (led_curr_color.g > led_fade_color.g) {
			uint8_t diff = led_curr_color.g - led_fade_color.g;
			led_curr_color.g -= MIN(diff, led_fadespeed);
		}
		else {
			uint8_t diff = led_fade_color.g - led_curr_color.g;
			led_curr_color.g += MIN(diff, led_fadespeed);
		}
	}
	if (led_curr_color.b != led_fade_color.b) {
		if (led_curr_color.b > led_fade_color.b) {
			uint8_t diff = led_curr_color.b - led_fade_color.b;
			led_curr_color.b -= MIN(diff, led_fadespeed);
		}
		else {
			uint8_t diff = led_fade_color.b - led_curr_color.b;
			led_curr_color.b += MIN(diff, led_fadespeed);
		}
	}

	//TODO: set led_fade=0  when fade_color is reached
}

void _CYZ_RGB_pulse() {
	if (++cyz_rgb.pulse_count == 0) {
		if (led_curr_color.r > 0) RED_LED_ON;
		if (led_curr_color.g > 0) GRN_LED_ON;
		if (led_curr_color.b > 0) BLU_LED_ON;
		if(led_fade==1) {
			__CYZ_RGB_fade_step();
		}
	}
	if (led_curr_color.r != 255 && cyz_rgb.pulse_count == led_curr_color.r) RED_LED_OFF;
	if (led_curr_color.g != 255 && cyz_rgb.pulse_count == led_curr_color.g) GRN_LED_OFF;
	if (led_curr_color.b != 255 && cyz_rgb.pulse_count == led_curr_color.b) BLU_LED_OFF;
}

void CYZ_RGB_init() {
	cyz_rgb.pulse_count = 0xFF;
	led_fade = 0;
	led_fadespeed = 1;
	PWM_DDR |= 1<<PINRED;
	PWM_DDR |= 1<<PINGRN;
	PWM_DDR |= 1<<PINBLU;
}
