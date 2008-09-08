#include "minunit.h"
#include "../cyz/cyz_cmd.h"
Color led_curr_color;
Color led_fade_color;
uint8_t led_fade;
uint8_t led_fadespeed;
static char* test_execute_go_to_rgb() {
	CYZ_CMD_init();
	uint8_t cmd[] = {'n', 255, 0 ,0};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(255, led_curr_color.r);
	return 0;
}

static char * test_cyz_cmd() {
	mu_run_test(test_execute_go_to_rgb);
	return 0;
}
