#include "cyz_rgb.h"

extern Color led_curr_color;
extern Color led_fade_color;
extern uint8_t led_fade;
extern uint8_t led_fadespeed;

void _CYZ_RGB_fade_step_component(uint8_t* current, uint8_t* target) {
	if (*current != *target) {
		if (*current > *target) {
			uint8_t diff = *current - *target;
			*current -= MIN(diff, led_fadespeed);
		}
		else {
			uint8_t diff = *target - *current;
			*current += MIN(diff, led_fadespeed);
		}
	}
}

void CYZ_RGB_init() {
	led_fade = 0;
	led_fadespeed = 1;
	PWM_DDR_R |= 1<<PINRED;
	PWM_DDR_G |= 1<<PINGRN;
	PWM_DDR_B |= 1<<PINBLU;
}
