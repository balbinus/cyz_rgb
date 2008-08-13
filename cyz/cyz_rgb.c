#include "cyz_rgb.h"

void _CYZ_RGB_init() {
	PWM_DDR |= 1<<PINRED;
	PWM_DDR |= 1<<PINGRN;
	PWM_DDR |= 1<<PINBLU;
}

void _CYZ_RGB_set_color(Cyz_rgb* this, unsigned char r, unsigned char g, unsigned char b) {
	this->fade = 0;
	this->color.r = r;
	this->color.g = g;
	this->color.b = b;
}

void _CYZ_RGB_set_fade_color(Cyz_rgb* this, unsigned char r, unsigned char g, unsigned char b) {
	this->fade = 1;
	this->fade_color.r = r;
	this->fade_color.g = g;
	this->fade_color.b = b;
}

void __CYZ_RGB_fade_step(Cyz_rgb* this) {
	if (this->color.r!=this->fade_color.r) { this->color.r += ((this->color.r>this->fade_color.r) ? -1 : +1); }
	if (this->color.g!=this->fade_color.g) { this->color.g += ((this->color.g>this->fade_color.g) ? -1 : +1); }
	if (this->color.b!=this->fade_color.b) { this->color.b += ((this->color.b>this->fade_color.b) ? -1 : +1); }
	//TODO: set this->fade=0  when fade_color is reached
}

void _CYZ_RGB_pulse(Cyz_rgb* this) {
	if (++this->pulse_count == 0) { RED_LED_ON; GRN_LED_ON; BLU_LED_ON; }
	if (this->pulse_count == this->color.r) RED_LED_OFF;
	if (this->pulse_count == this->color.g) GRN_LED_OFF;
	if (this->pulse_count == this->color.b) BLU_LED_OFF;
	if (this->pulse_count == 0 && this->fade) {
		__CYZ_RGB_fade_step(this);
	}
}

Cyz_rgb* CYZ_RGB_GET_INSTANCE() {
	 Cyz_rgb* instance = (Cyz_rgb*) malloc(sizeof(struct CYZ_RGB));
	 instance->init = _CYZ_RGB_init;
	 instance->set_color = (void*)_CYZ_RGB_set_color;
	 instance->set_fade_color = (void*)_CYZ_RGB_set_fade_color;
	 instance->pulse_count = 0xFF;
	 instance->fade = 0;
	 instance->pulse = (void*)_CYZ_RGB_pulse;
	 return instance;
}
