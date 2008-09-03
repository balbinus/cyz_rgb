#include "cyz_rgb.h"

Cyz_rgb cyz_rgb;

Cyz_rgb* _CYZ_RGB_get() {
	return &cyz_rgb;
}

void _CYZ_RGB_set_color(unsigned char r, unsigned char g, unsigned char b) {
	cyz_rgb.fade = 0;
	cyz_rgb.color.r = r;
	cyz_rgb.color.g = g;
	cyz_rgb.color.b = b;
}

void _CYZ_RGB_set_fade_color(unsigned char r, unsigned char g, unsigned char b) {
	cyz_rgb.fade = 1;
	cyz_rgb.fade_color.r = r;
	cyz_rgb.fade_color.g = g;
	cyz_rgb.fade_color.b = b;
}

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

void __CYZ_RGB_fade_step() {
	if (cyz_rgb.color.r != cyz_rgb.fade_color.r) {
		if (cyz_rgb.color.r > cyz_rgb.fade_color.r) {
			uint8_t diff = cyz_rgb.color.r - cyz_rgb.fade_color.r;
			cyz_rgb.color.r -= MIN(diff, cyz_rgb.fadespeed);
		}
		else {
			uint8_t diff = cyz_rgb.fade_color.r - cyz_rgb.color.r;
			cyz_rgb.color.r += MIN(diff, cyz_rgb.fadespeed);
		}
	}
	if (cyz_rgb.color.g != cyz_rgb.fade_color.g) {
		if (cyz_rgb.color.g > cyz_rgb.fade_color.g) {
			uint8_t diff = cyz_rgb.color.g - cyz_rgb.fade_color.g;
			cyz_rgb.color.g -= MIN(diff, cyz_rgb.fadespeed);
		}
		else {
			uint8_t diff = cyz_rgb.fade_color.g - cyz_rgb.color.g;
			cyz_rgb.color.g += MIN(diff, cyz_rgb.fadespeed);
		}
	}
	if (cyz_rgb.color.b != cyz_rgb.fade_color.b) {
		if (cyz_rgb.color.b > cyz_rgb.fade_color.b) {
			uint8_t diff = cyz_rgb.color.b - cyz_rgb.fade_color.b;
			cyz_rgb.color.b -= MIN(diff, cyz_rgb.fadespeed);
		}
		else {
			uint8_t diff = cyz_rgb.fade_color.b - cyz_rgb.color.b;
			cyz_rgb.color.b += MIN(diff, cyz_rgb.fadespeed);
		}
	}

	//TODO: set cyz_rgb.fade=0  when fade_color is reached
}

void _CYZ_RGB_pulse() {
	if (++cyz_rgb.pulse_count == 0) {
		if (cyz_rgb.color.r > 0) RED_LED_ON;
		if (cyz_rgb.color.g > 0) GRN_LED_ON;
		if (cyz_rgb.color.b > 0) BLU_LED_ON;
		if(cyz_rgb.fade==1) {
			__CYZ_RGB_fade_step();
		}
	}
	if (cyz_rgb.color.r != 255 && cyz_rgb.pulse_count == cyz_rgb.color.r) RED_LED_OFF;
	if (cyz_rgb.color.g != 255 && cyz_rgb.pulse_count == cyz_rgb.color.g) GRN_LED_OFF;
	if (cyz_rgb.color.b != 255 && cyz_rgb.pulse_count == cyz_rgb.color.b) BLU_LED_OFF;
}

Cyz_rgb* CYZ_RGB_GET_INSTANCE() {
	cyz_rgb.pulse_count = 0xFF;
	cyz_rgb.fade = 0;
	cyz_rgb.fadespeed = 1;
	PWM_DDR |= 1<<PINRED;
	PWM_DDR |= 1<<PINGRN;
	PWM_DDR |= 1<<PINBLU;
	return &cyz_rgb;
}
