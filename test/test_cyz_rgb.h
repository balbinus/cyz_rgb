#include "minunit.h"
#include "../cyz/cyz_rgb.h"

static char * test_cyz_rgb_init() {
	led_fade = 99;
	led_fadespeed = 89;
	pin_red = 2;
	pin_grn = 4;
	pin_blu = 8;
	CYZ_RGB_init();
	mu_assert_eq(0, led_fade);
	mu_assert_eq(1, led_fadespeed);
	uint8_t pwm = 0;
	pwm |= (1<<PINRED | 1<<PINGRN | 1<<PINBLU);

	mu_assert_eq(PWM_DDR, pwm);
	return 0;
}

static char * test_cyz_rgb() {
	mu_run_test(test_cyz_rgb_init);
	return 0;
}
