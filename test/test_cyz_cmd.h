#include "minunit.h"
#include "../cyz/cyz_cmd.h"
Color led_curr_color;
Color led_fade_color;
uint8_t led_fade;
uint8_t led_fadespeed;

static char* test_execute_go_to_rgb() {
	CYZ_CMD_init();
	uint8_t cmd[] = {'n', 33, 66 ,99};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(33, led_curr_color.r);
	mu_assert_eq(66, led_curr_color.g);
	mu_assert_eq(99, led_curr_color.b);
	return 0;
}

static char* test_execute_fade_to_rgb() {
	CYZ_CMD_init();
	led_fade = 44;
	led_fade_color.r = led_fade_color.g = led_fade_color.b = 111;
	uint8_t cmd[] = {'c', 33, 66 ,99};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(1, led_fade);
	mu_assert_eq(33, led_fade_color.r);
	mu_assert_eq(66, led_fade_color.g);
	mu_assert_eq(99, led_fade_color.b);
	return 0;
}

static char* test_execute_get_addr() {
	CYZ_CMD_init();
	cyz_cmd.addr = 99;
	uint8_t cmd[] = {'a'};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(99, ring_buffer_pop(cyz_cmd.send_buffer));
	return 0;
}

static char* test_execute_set_addr() {
	CYZ_CMD_init();
	cyz_cmd.addr = 0x0a;
	uint8_t cmd[] = {'A', 0x0b, 0x0d, 0x0d, 0x0b };
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(0x0b, cyz_cmd.addr);
	return 0;
}

static char* test_execute_set_addr_addr_nomatch() {
	CYZ_CMD_init();
	cyz_cmd.addr = 0x0a;
	uint8_t cmd[] = {'A', 0x0b, 0x0d, 0x0d, 0x0d };
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(0x0a, cyz_cmd.addr);
	return 0;
}

static char* test_execute_set_addr_check_nomatch() {
	CYZ_CMD_init();
	cyz_cmd.addr = 0x0a;
	uint8_t cmd[] = {'A', 0x0b, 0x0d, 0x0e, 0x0b };
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(0x0a, cyz_cmd.addr);
	return 0;
}

static char* test_execute_set_addr_check_incorrect() {
	CYZ_CMD_init();
	cyz_cmd.addr = 0x0a;
	uint8_t cmd[] = {'A', 0x0b, 0x0d, 0x0e, 0x0b };
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(0x0a, cyz_cmd.addr);
	return 0;
}

static char* test_execute_get_rgb() {
	CYZ_CMD_init();
	led_curr_color.r = 3;
	led_curr_color.g = 6;
	led_curr_color.b = 9;
	uint8_t cmd[] = {'g'};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(3, ring_buffer_pop(cyz_cmd.send_buffer));
	mu_assert_eq(6, ring_buffer_pop(cyz_cmd.send_buffer));
	mu_assert_eq(9, ring_buffer_pop(cyz_cmd.send_buffer));

	return 0;
}

static char* test_execute_set_fadespeed() {
	CYZ_CMD_init();
	led_fadespeed = 99;
	uint8_t cmd[] = {'f', 44 };
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(44, led_fadespeed);
	return 0;
}

static char* test_execute_stop_script() {
	CYZ_CMD_init();
	cyz_cmd.play_script = 2;
	uint8_t cmd[] = {'o'};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(0, cyz_cmd.play_script);
	return 0;
}

static char* test_execute_get_firmware_version() {
	CYZ_CMD_init();
	uint8_t cmd[] = {'Z'};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(0, ring_buffer_pop(cyz_cmd.send_buffer));
	mu_assert_eq(2, ring_buffer_pop(cyz_cmd.send_buffer));
	return 0;
}

static char* test_execute_set_len_repeats() {
	CYZ_CMD_init();
	cyz_cmd.script_length = 230;
	cyz_cmd.script_repeats = 133;
	uint8_t cmd[] = {'L', 0, 44, 55 };
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(44, cyz_cmd.script_length);
	mu_assert_eq(55, cyz_cmd.script_repeats);
	return 0;
}

static char* test_execute_set_timeadjiust() {
	CYZ_CMD_init();
	cyz_cmd.timeadjust = 99;
	uint8_t cmd[] = {'t', 44 };
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(44, cyz_cmd.timeadjust);
	return 0;
}

static char* test_execute_play_light_script() {
	CYZ_CMD_init();
	cyz_cmd.play_script = 33;
	cyz_cmd.script_length = 44;
	cyz_cmd.script_repeats = 55;
	uint8_t cmd[] = {'p', 12, 22, 52};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(13, cyz_cmd.play_script);
	mu_assert_eq(22, cyz_cmd.script_repeats);
	mu_assert_eq(52, cyz_cmd.script_pos);
	return 0;
}

static char* test_prng() {
	int i;
	for (i=0; i<1000; i++) {
		mu_assert("prng < 10", _CYZ_CMD_prng(10) < 10);
		mu_assert("prng < 100", _CYZ_CMD_prng(100) < 100);
		mu_assert("prng < 255", _CYZ_CMD_prng(255) < 255);
	}
	return 0;
}

static char* test_execute_fade_to_rnd_rgb() {
	CYZ_CMD_init();
	led_curr_color.r = 10;
	led_curr_color.g = 11;
	led_curr_color.b = 12;
	uint8_t cmd[] = {'C',10,11,12};
	_CYZ_CMD_execute(cmd);
	mu_assert("fade.r >= 10", led_fade_color.r >= 10);
	mu_assert("fade.g >= 11", led_fade_color.g >= 11);
	mu_assert("fade.b >= 12", led_fade_color.b >= 12);
	return 0;
}

static char * test_cyz_cmd() {
	mu_run_test(test_execute_go_to_rgb);
	mu_run_test(test_execute_fade_to_rgb);
	mu_run_test(test_execute_get_addr);
	mu_run_test(test_execute_set_addr);
	mu_run_test(test_execute_set_addr_addr_nomatch);
	mu_run_test(test_execute_set_addr_check_nomatch);
	mu_run_test(test_execute_set_addr_check_incorrect);
	mu_run_test(test_execute_get_rgb);
	mu_run_test(test_execute_set_fadespeed);
	mu_run_test(test_execute_stop_script);
	mu_run_test(test_execute_get_firmware_version);
	mu_run_test(test_execute_set_len_repeats);
	mu_run_test(test_execute_set_timeadjiust);
	mu_run_test(test_execute_play_light_script);
	mu_run_test(test_prng);
	mu_run_test(test_execute_fade_to_rnd_rgb);
	return 0;
}
