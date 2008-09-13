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
	cyz_cmd.tick_count = 99;
	uint8_t cmd[] = {'p', 12, 22, 52};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(13, cyz_cmd.play_script);
	mu_assert_eq(22, cyz_cmd.script_repeats);
	mu_assert_eq(52, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.tick_count);
	return 0;
}

static char* test_prng() {
	int i;
	for (i=0; i<1000; i++) {
		mu_assert("prng < 10", CYZ_CMD_prng(10) < 10);
		mu_assert("prng < 100", CYZ_CMD_prng(100) < 100);
		mu_assert("prng < 255", CYZ_CMD_prng(255) < 255);
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

static char * test_execute_write_script_line() {
	CYZ_CMD_init();
	uint8_t cmd[] = {'W', 0,5,133,'X',190,160,130};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(5, mock_buf[5]); //lineno
	mu_assert_eq(133, mock_buf[0]); //dur
	mu_assert_eq('X', mock_buf[1]); //cmd[0]
	mu_assert_eq(190, mock_buf[2]); //cmd[1]
	mu_assert_eq(160, mock_buf[3]); //cmd[2]
	mu_assert_eq(130, mock_buf[4]); //cmd[3]

	return 0;
}

static char * test_execute_set_boot_parms_mode_play() {
	CYZ_CMD_init();
	uint8_t cmd[] = {'B', 1, 5, 'r', 'f', 't'};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(CYZ_CMD_BOOTP_MAGIC, mock_buf[0]);
	mu_assert_eq(5+1, mock_buf[1]);
	mu_assert_eq('f', mock_buf[3]);
	mu_assert_eq('t', mock_buf[4]);
	mu_assert_eq(0, mock_buf[5]);
	return 0;
}

static char * test_execute_set_boot_parms_mode_no_play() {
	CYZ_CMD_init();
	uint8_t cmd[] = {'B', 0, 5, 'r', 'f', 't'};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq(CYZ_CMD_BOOTP_MAGIC, mock_buf[0]);
	mu_assert_eq(0, mock_buf[1]);
	mu_assert_eq('f', mock_buf[3]);
	mu_assert_eq('t', mock_buf[4]);
	mu_assert_eq(0, mock_buf[5]);
	return 0;

	return 0;
}

static char * test_execute_get_script_line() {
	CYZ_CMD_init();
	uint8_t cmd[] = {'R', 0, 2};
	mock_buf[0] = 'd';
	mock_buf[1] = 'c';
	mock_buf[2] = 'A';
	mock_buf[3] = 'B';
	mock_buf[4] = 'C';
	_CYZ_CMD_execute(cmd);
	mu_assert_eq('d', ring_buffer_pop(cyz_cmd.send_buffer));
	mu_assert_eq('c', ring_buffer_pop(cyz_cmd.send_buffer));
	mu_assert_eq('A', ring_buffer_pop(cyz_cmd.send_buffer));
	mu_assert_eq('B', ring_buffer_pop(cyz_cmd.send_buffer));
	mu_assert_eq('C', ring_buffer_pop(cyz_cmd.send_buffer));

	return 0;
}

static char * test_load_boot_params_if_magic() {
	CYZ_CMD_init();
	led_fadespeed = 0;
	mock_buf[0] = CYZ_CMD_BOOTP_MAGIC;
	mock_buf[1] = 34;
	mock_buf[2] = 45;
	mock_buf[3] = 56;
	mock_buf[4] = 67;
	CYZ_CMD_load_boot_params();
	mu_assert_eq(34, cyz_cmd.play_script);
	mu_assert_eq(45, cyz_cmd.script_repeats);
	mu_assert_eq(56, led_fadespeed);
	mu_assert_eq(67, cyz_cmd.timeadjust);
	return 0;
}

static char * test_load_boot_params_if_no_magic() {
	CYZ_CMD_init();
	led_fadespeed = 0;
	mock_buf[0] = 0;
	mock_buf[1] = 34;
	mock_buf[2] = 45;
	mock_buf[3] = 56;
	mock_buf[4] = 67;
	CYZ_CMD_load_boot_params();
	mu_assert_eq(0, cyz_cmd.play_script);
	mu_assert_eq(0, cyz_cmd.script_repeats);
	mu_assert_eq(0, led_fadespeed);
	mu_assert_eq(0, cyz_cmd.timeadjust);
	return 0;
}

static char * test_init() {
	cyz_cmd.addr = 11;
	cyz_cmd.script_length = 22;
	cyz_cmd.script_pos = 33;
	cyz_cmd.script_repeats = 33;
	cyz_cmd.script_repeated = 44;
	cyz_cmd.timeadjust = 55;
	cyz_cmd.tick_count = 66;
	cyz_cmd.send_buffer.idx_end = 77;
	cyz_cmd.send_buffer.idx_start = 88;
	CYZ_CMD_init();
	mu_assert_eq(0x0d, cyz_cmd.addr);
	mu_assert_eq(0, cyz_cmd.script_length);
	mu_assert_eq(0, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(0, cyz_cmd.timeadjust);
	mu_assert_eq(0, cyz_cmd.tick_count);
	mu_assert_eq(0, cyz_cmd.send_buffer.idx_end);
	mu_assert_eq(0, cyz_cmd.send_buffer.idx_start);
	return 0;
}

static char * test_tick() {
	CYZ_CMD_init();
	cyz_cmd.play_script = 1;
	mock_buf[0] = 255;
	CYZ_CMD_tick();
	mu_assert_eq(1, cyz_cmd.tick_count);
	CYZ_CMD_tick();
	mu_assert_eq(2, cyz_cmd.tick_count);

	cyz_cmd.tick_count = 0;
	int i;
	for (i=0; i<256; i++) {
		CYZ_CMD_tick();
	}
	mu_assert_eq(0, cyz_cmd.tick_count);
	return 0;
}

static char * test_tick_play() {
	CYZ_CMD_init();
	cyz_cmd.play_script = 1;
	cyz_cmd.script_length = 5;
	cyz_cmd.tick_count = 0;
	mock_buf[0] = 100;
	CYZ_CMD_tick();
	mu_assert_eq(-100, cyz_cmd.tick_count);
	int i;
	for (i=0; i<100; i++) {
		CYZ_CMD_tick();
	}
	mu_assert_eq(0, cyz_cmd.tick_count);
	mock_buf[0] = 44;
	CYZ_CMD_tick();
	mu_assert_eq(-44, cyz_cmd.tick_count);
	cyz_cmd.tick_count = 0;
	mock_buf[0] = 255;
	for (i=0; i<255; i++) {
		CYZ_CMD_tick();
	}
	mu_assert_eq(255, cyz_cmd.tick_count);
	return 0;
}

static char * test_tick_no_play() {
	CYZ_CMD_init();
	mu_assert_eq(0, cyz_cmd.tick_count);
	CYZ_CMD_tick();
	mu_assert_eq(0, cyz_cmd.tick_count);
	return 0;
}

static char * test_play_next_script_line_eeprom() {
	CYZ_CMD_init();
	cyz_cmd.play_script = 1;
	cyz_cmd.timeadjust = 0;
	cyz_cmd.script_length = 5;
	cyz_cmd.script_repeats = 0;
	mock_buf[0] = 66;
	mu_assert_eq(0, cyz_cmd.script_pos);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(1, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(0, mock_buf[5]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(2, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(1, mock_buf[5]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(3, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(2, mock_buf[5]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(4, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(3, mock_buf[5]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(0, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(4, mock_buf[5]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(1, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(0, mock_buf[5]);

	int i = 0;
	cyz_cmd.script_pos = 0;
	for(i=0; i<1000; i++) {
		mock_buf[0] = i;
		mock_buf[1] = 'n';
		mock_buf[2] = i;
		mock_buf[3] = i+1;
		mock_buf[4] = i+2;
		mu_assert_eq(i, CYZ_CMD_play_next_script_line());
		mu_assert_eq(i, led_curr_color.r);
		mu_assert_eq(i+1, led_curr_color.g);
		mu_assert_eq(i+2, led_curr_color.b);
		mu_assert_eq((i+1)%5, cyz_cmd.script_pos);
		mu_assert_eq(0, cyz_cmd.script_repeated);
	}

	cyz_cmd.script_pos = 0;
	cyz_cmd.script_repeats = 10;
	for(i=0; i<50; i++) {
		mock_buf[0] = i;
		mock_buf[1] = 'n';
		mock_buf[2] = i;
		mock_buf[3] = i+1;
		mock_buf[4] = i+2;
		mu_assert_eq(i, CYZ_CMD_play_next_script_line());
		mu_assert_eq(i, led_curr_color.r);
		mu_assert_eq(i+1, led_curr_color.g);
		mu_assert_eq(i+2, led_curr_color.b);
		mu_assert_eq((i+1)%5, cyz_cmd.script_pos);
		if (i<49) {
			mu_assert_eq(((i+1)/5), cyz_cmd.script_repeated);
		} else {
			mu_assert_eq(0, cyz_cmd.script_repeated);
		}
	}

	return 0;
}

static char * test_play_next_script_line_progmem() {
	CYZ_CMD_init();
	cyz_cmd.play_script = 3;
	cyz_cmd.timeadjust = 0;
	cyz_cmd.script_length = 5;
	cyz_cmd.script_repeats = 0;
	progmem_mock_buf[0] = 66;
	mu_assert_eq(0, cyz_cmd.script_pos);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(1, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(0, progmem_mock_buf[5]);
	mu_assert_eq(1, progmem_mock_buf[6]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(2, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(1, progmem_mock_buf[5]);
	mu_assert_eq(1, progmem_mock_buf[6]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(3, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(2, progmem_mock_buf[5]);
	mu_assert_eq(1, progmem_mock_buf[6]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(4, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(3, progmem_mock_buf[5]);
	mu_assert_eq(1, progmem_mock_buf[6]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(0, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(4, progmem_mock_buf[5]);
	mu_assert_eq(1, progmem_mock_buf[6]);
	mu_assert_eq(66, CYZ_CMD_play_next_script_line());
	mu_assert_eq(1, cyz_cmd.script_pos);
	mu_assert_eq(0, cyz_cmd.script_repeated);
	mu_assert_eq(0, progmem_mock_buf[5]);
	mu_assert_eq(1, progmem_mock_buf[6]);

	int i = 0;
	cyz_cmd.script_pos = 0;
	for(i=0; i<1000; i++) {
		progmem_mock_buf[0] = i;
		progmem_mock_buf[1] = 'n';
		progmem_mock_buf[2] = i;
		progmem_mock_buf[3] = i+1;
		progmem_mock_buf[4] = i+2;
		mu_assert_eq(i, CYZ_CMD_play_next_script_line());
		mu_assert_eq(i, led_curr_color.r);
		mu_assert_eq(i+1, led_curr_color.g);
		mu_assert_eq(i+2, led_curr_color.b);
		mu_assert_eq((i+1)%5, cyz_cmd.script_pos);
		mu_assert_eq(0, cyz_cmd.script_repeated);
	}

	cyz_cmd.script_pos = 0;
	cyz_cmd.script_repeats = 10;
	for(i=0; i<50; i++) {
		progmem_mock_buf[0] = i;
		progmem_mock_buf[1] = 'n';
		progmem_mock_buf[2] = i;
		progmem_mock_buf[3] = i+1;
		progmem_mock_buf[4] = i+2;
		mu_assert_eq(i, CYZ_CMD_play_next_script_line());
		mu_assert_eq(i, led_curr_color.r);
		mu_assert_eq(i+1, led_curr_color.g);
		mu_assert_eq(i+2, led_curr_color.b);
		mu_assert_eq((i+1)%5, cyz_cmd.script_pos);
		if (i<49) {
			mu_assert_eq(((i+1)/5), cyz_cmd.script_repeated);
		} else {
			mu_assert_eq(0, cyz_cmd.script_repeated);
		}
	}

	return 0;
}

static char * test_receive_one_byte() {
	CYZ_CMD_init();
	CYZ_CMD_receive_one_byte('n');
	mu_assert_eq('n', cyz_cmd.rcv_cmd_buf[0]);
	mu_assert_eq(1, cyz_cmd.rcv_cmd_buf_cnt);
	mu_assert_eq(4, cyz_cmd.rcv_cmd_len);

	CYZ_CMD_receive_one_byte('x');
	mu_assert_eq('x', cyz_cmd.rcv_cmd_buf[1]);
	mu_assert_eq(2, cyz_cmd.rcv_cmd_buf_cnt);
	mu_assert_eq(4, cyz_cmd.rcv_cmd_len);

	CYZ_CMD_receive_one_byte('y');
	mu_assert_eq('y', cyz_cmd.rcv_cmd_buf[2]);
	mu_assert_eq(3, cyz_cmd.rcv_cmd_buf_cnt);
	mu_assert_eq(4, cyz_cmd.rcv_cmd_len);

	CYZ_CMD_receive_one_byte('z');
	mu_assert_eq('z', cyz_cmd.rcv_cmd_buf[3]);
	mu_assert_eq(0, cyz_cmd.rcv_cmd_buf_cnt);
	mu_assert_eq(4, cyz_cmd.rcv_cmd_len);

	mu_assert_eq('x', led_curr_color.r);
	mu_assert_eq('y', led_curr_color.g);
	mu_assert_eq('z', led_curr_color.b);

	CYZ_CMD_receive_one_byte('c');
	mu_assert_eq('c', cyz_cmd.rcv_cmd_buf[0]);
	mu_assert_eq(1, cyz_cmd.rcv_cmd_buf_cnt);
	mu_assert_eq(4, cyz_cmd.rcv_cmd_len);

	CYZ_CMD_receive_one_byte('j');
	mu_assert_eq('j', cyz_cmd.rcv_cmd_buf[1]);
	mu_assert_eq(2, cyz_cmd.rcv_cmd_buf_cnt);
	mu_assert_eq(4, cyz_cmd.rcv_cmd_len);

	CYZ_CMD_receive_one_byte('t');
	mu_assert_eq('t', cyz_cmd.rcv_cmd_buf[2]);
	mu_assert_eq(3, cyz_cmd.rcv_cmd_buf_cnt);
	mu_assert_eq(4, cyz_cmd.rcv_cmd_len);

	CYZ_CMD_receive_one_byte('p');
	mu_assert_eq('p', cyz_cmd.rcv_cmd_buf[3]);
	mu_assert_eq(0, cyz_cmd.rcv_cmd_buf_cnt);
	mu_assert_eq(4, cyz_cmd.rcv_cmd_len);

	mu_assert_eq('j', led_fade_color.r);
	mu_assert_eq('t', led_fade_color.g);
	mu_assert_eq('p', led_fade_color.b);
	return 0;
}

static char * test_execute_get_dbg() {
	CYZ_CMD_init();
	cyz_cmd.dbg[0] = 'a';
	cyz_cmd.dbg[1] = 'b';
	uint8_t cmd[] = {CMD_GET_DBG};
	_CYZ_CMD_execute(cmd);
	mu_assert_eq('a', ring_buffer_pop(cyz_cmd.send_buffer));
	mu_assert_eq('b', ring_buffer_pop(cyz_cmd.send_buffer));
	return 0;
}

static char * test_cyz_cmd() {
	mu_run_test(test_execute_go_to_rgb);
	mu_run_test(test_execute_fade_to_rgb);
	mu_run_test(test_execute_fade_to_rnd_rgb);
	// CMD_FADE_TO_HSB
	// CMD_FADE_TO_RND_HSB
	mu_run_test(test_execute_write_script_line);
	mu_run_test(test_execute_play_light_script);
	mu_run_test(test_execute_stop_script);
	mu_run_test(test_execute_set_boot_parms_mode_play);
	mu_run_test(test_execute_set_boot_parms_mode_no_play);
	mu_run_test(test_execute_set_timeadjiust);
	mu_run_test(test_execute_set_fadespeed);
	mu_run_test(test_execute_set_len_repeats);
	mu_run_test(test_execute_set_addr);
	mu_run_test(test_execute_set_addr_addr_nomatch);
	mu_run_test(test_execute_set_addr_check_nomatch);
	mu_run_test(test_execute_set_addr_check_incorrect);
	mu_run_test(test_execute_get_addr);
	mu_run_test(test_execute_get_rgb);
	mu_run_test(test_execute_get_script_line);
	mu_run_test(test_execute_get_firmware_version);
	mu_run_test(test_execute_get_dbg);
	mu_run_test(test_prng);
	mu_run_test(test_play_next_script_line_eeprom);
	mu_run_test(test_play_next_script_line_progmem);
	mu_run_test(test_receive_one_byte);
	mu_run_test(test_load_boot_params_if_magic);
	mu_run_test(test_load_boot_params_if_no_magic);
	mu_run_test(test_tick);
	mu_run_test(test_tick_play);
	mu_run_test(test_tick_no_play);
	mu_run_test(test_init);

	return 0;
}
