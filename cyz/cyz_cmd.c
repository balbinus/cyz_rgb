#include "cyz_cmd.h"
#include <limits.h>
#include "color.h"

boot_parms EEMEM EEbbotp;
uint8_t EEMEM EEaddr;
script EEMEM EEscript;

const script fl_script_rgb PROGMEM = {
		3, // number of lines
		0, // number of repeats
		{
			{ 50, {'n', 0xff,0x00,0x00}},
			{ 50, {'n', 0x00,0xff,0x00}},
			{ 50, {'n', 0x00,0x00,0xff}},
		}
};

const script* scripts[] PROGMEM = {
		&fl_script_rgb,   // 1
};

void CYZ_CMD_init() {
	cyz_cmd.rcv_cmd_buf_cnt = 0;
	cyz_cmd.rcv_cmd_len = 0xFF;
	cyz_cmd.play_script = 0;
	cyz_cmd.script_length = 0;
	cyz_cmd.script_pos = 0;
	cyz_cmd.script_repeats = 0;
	cyz_cmd.script_repeated = 0;
	cyz_cmd.timeadjust = 0;
	cyz_cmd.addr = 0x0d;
	cyz_cmd.tick_count = 0;
	cyz_cmd.send_buffer.idx_start = 0;
	cyz_cmd.send_buffer.idx_end = 0;
	cyz_cmd.dbg[0] = 42;
}

/* returns the length of the command, command+payload. */
/* example "fadeToColor" is 'f',R,G,B: the returned length is 4 */
/* 0xFF means error */
uint8_t CYZ_CMD_get_cmd_len (uint8_t cmd) {
	switch(cmd) {
	case CMD_GO_TO_RGB:
		return 4;
	case CMD_FADE_TO_RGB:
		return 4;
	case CMD_FADE_TO_RND_RGB:
		return 4;
	case CMD_FADE_TO_HSB:
		return 4;
	case CMD_FADE_TO_RND_HSB:
		return 4;
	case CMD_WRITE_SCRIPT_LINE:
		return 8;
	case CMD_PLAY_LIGHT_SCRIPT:
		return 4;
	case CMD_STOP_SCRIPT:
		return 1;
	case CMD_SET_BOOT_PARMS:
		return 6;
	case CMD_SET_TIMEADJUST:
		return 2;
	case CMD_SET_FADESPEED:
		return 2;
	case CMD_SET_LEN_RPTS:
		return 4;
	case CMD_SET_ADDR:
		return 5;
	case CMD_GET_ADDR:
		return 1;
	case CMD_GET_RGB:
		return 1;
	case CMD_GET_SCRIPT_LINE:
		return 3;
	case CMD_GET_FIRMWARE_VERSION:
		return 1;
	case CMD_GET_DBG:
		return 1;
	}
	return 0xFF;
}

void _CYZ_CMD_execute(uint8_t* cmd) {
	switch (cmd[0]) {
	case CMD_GET_DBG:
		ring_buffer_push_array(&cyz_cmd.send_buffer, cyz_cmd.dbg, 8);
		break;
	case CMD_GO_TO_RGB:
		led_fade = 0;
		led_curr_color.r = cmd[1];
		led_curr_color.g = cmd[2];
		led_curr_color.b = cmd[3];
		break;
	case CMD_FADE_TO_RGB:
		led_fade = 1;
		led_fade_color.r = cmd[1];
		led_fade_color.g = cmd[2];
		led_fade_color.b = cmd[3];
		break;
	case CMD_FADE_TO_RND_RGB:
		led_fade = 1;
		led_fade_color.r = led_curr_color.r + CYZ_CMD_prng(cmd[1]);
		led_fade_color.g = led_curr_color.g + CYZ_CMD_prng(cmd[2]);
		led_fade_color.b = led_curr_color.b + CYZ_CMD_prng(cmd[3]);
		break;
	case CMD_FADE_TO_HSB:
		led_fade = 1;
		color_hsv_to_rgb(cmd[1], cmd[2], cmd[3], &led_fade_color.r, &led_fade_color.g, &led_fade_color.b);
		break;
	case CMD_FADE_TO_RND_HSB:
	{
		led_fade = 1;
		uint8_t h,s,v;
		color_rgb_to_hsv(led_curr_color, &h, &s, &v);
		h += CYZ_CMD_prng(cmd[1]);
		s += CYZ_CMD_prng(cmd[2]);
		v += CYZ_CMD_prng(cmd[3]);
		color_hsv_to_rgb(h, s, v, &led_fade_color.r, &led_fade_color.g, &led_fade_color.b);
	}
	break;
	case CMD_WRITE_SCRIPT_LINE:
	{
		if (cmd[2] > MAX_SCRIPT_LEN-1)
			break;
		//TODO: cmd[1] (script id) is ignored, only one script can be written

		script_line tmp;
		tmp.dur = cmd[3];
		tmp.cmd[0] = cmd[4];
		tmp.cmd[1] = cmd[5];
		tmp.cmd[2] = cmd[6];
		tmp.cmd[3] = cmd[7];
		EEPROM_write_script_line(tmp, cmd[2]); // cmd[2] is line number
	}
	break;
	case CMD_PLAY_LIGHT_SCRIPT:
		cyz_cmd.play_script = cmd[1]+1;
		cyz_cmd.script_repeats = cmd[2];
		cyz_cmd.script_pos = cmd[3];
		cyz_cmd.tick_count = 0;
		break;
	case CMD_STOP_SCRIPT:
		cyz_cmd.play_script = 0;
		break;
	case CMD_SET_BOOT_PARMS:
	{
		boot_parms temp;
		temp.magic = CYZ_CMD_BOOTP_MAGIC;
		if (cmd[1] > 0) {
			temp.play_script = cmd[2]+1;
		}
		else {
			temp.play_script = 0;
		}
		temp.repeats = cmd[3];
		temp.fadespeed = cmd[4];
		temp.timeadjust = cmd[5];
		EEPROM_write_boot_parms(temp);
	}
	break;
	case CMD_SET_FADESPEED:
		if (cmd[1] != 0) {
			led_fadespeed = cmd[1];
		}
		break;
	case CMD_SET_TIMEADJUST:
		cyz_cmd.timeadjust = cmd[1];
		break;
	case CMD_SET_LEN_RPTS:
		//cmd[1] is script number, currently ignored, can only set script 0
		cyz_cmd.script_length = cmd[2];
		cyz_cmd.script_repeats = cmd[3];
		break;
	case CMD_SET_ADDR:
		if (cmd[2] == 0xd0 && cmd[3] == 0x0d && cmd[1] == cmd[4]) { //d00d!
			cyz_cmd.addr = cmd[1];
			EEPROM_write_addr(cyz_cmd.addr);
		}
		break;
	case CMD_GET_ADDR:
		ring_buffer_push(&cyz_cmd.send_buffer, cyz_cmd.addr);
		break;
	case CMD_GET_RGB:
		ring_buffer_push(&cyz_cmd.send_buffer, led_curr_color.r);
		ring_buffer_push(&cyz_cmd.send_buffer, led_curr_color.g);
		ring_buffer_push(&cyz_cmd.send_buffer, led_curr_color.b);
		break;
	case CMD_GET_SCRIPT_LINE:
	{
		//TODO, make other scripts readable
		//cmd[1] is ignore: only script 0 can be read
		script_line tmp;
		EEPROM_read_script_line(tmp, cmd[2]);
		ring_buffer_push(&cyz_cmd.send_buffer, tmp.dur);
		ring_buffer_push_array(&cyz_cmd.send_buffer, tmp.cmd, 4);
	}
	break;
	case CMD_GET_FIRMWARE_VERSION:
	{
		ring_buffer_push(&cyz_cmd.send_buffer, VERSION_MAJOR);
		ring_buffer_push(&cyz_cmd.send_buffer, VERSION_MINOR);
	}
	break;
	}
}

void CYZ_CMD_load_boot_params() {
	EEPROM_read_addr(cyz_cmd.addr);
	boot_parms temp;
	EEPROM_read_boot_parms(temp);
	if (temp.magic == CYZ_CMD_BOOTP_MAGIC) {
		if (temp.play_script > 0) {
			cyz_cmd.play_script = temp.play_script;
			cyz_cmd.script_repeats = temp.repeats;
			if (temp.fadespeed != 0) {
				led_fadespeed = temp.fadespeed;
			}
			cyz_cmd.timeadjust = temp.timeadjust;
		}
	}
}

/* to be invoked inside a timer, if script is playing */
uint8_t CYZ_CMD_play_next_script_line() {
	//TODO: load lines in memory only once
	script_line tmp;
	if (cyz_cmd.play_script-1 == 0) {
		EEPROM_read_script_line(tmp, cyz_cmd.script_pos);
	}
	else {
		PROGMEM_read_script_line(tmp, cyz_cmd.play_script-2, cyz_cmd.script_pos)
	}

	cyz_cmd.script_pos++;
	_CYZ_CMD_execute(tmp.cmd);

	if (cyz_cmd.script_pos == cyz_cmd.script_length) {
		cyz_cmd.script_pos = 0;
		if (cyz_cmd.script_repeats > 0 && ++cyz_cmd.script_repeated >= cyz_cmd.script_repeats){
			cyz_cmd.play_script = 0;
			cyz_cmd.script_repeated = 0;
		}
	}

	return tmp.dur + cyz_cmd.timeadjust;
}

void CYZ_CMD_receive_one_byte(uint8_t in) {
	cyz_cmd.rcv_cmd_buf[cyz_cmd.rcv_cmd_buf_cnt] = in;
	/* first byte contains the command, use it to decide length of payload */
	if (cyz_cmd.rcv_cmd_buf_cnt == 0) {
		cyz_cmd.rcv_cmd_len = CYZ_CMD_get_cmd_len(in);
	}

	if (cyz_cmd.rcv_cmd_buf_cnt==(cyz_cmd.rcv_cmd_len-1)) {
		_CYZ_CMD_execute(cyz_cmd.rcv_cmd_buf);
		cyz_cmd.rcv_cmd_buf_cnt = 0;
	}
	else {
		cyz_cmd.rcv_cmd_buf_cnt++;
	}
}

#define M 0x7FFFFFFF  // 2^31-1, the modulus used by the psuedo-random number generator prng().
uint8_t CYZ_CMD_prng(uint8_t range) {
	static uint8_t count = 1;
	if (range == 0) return 0;
	uint8_t x = ++count + cyz_cmd.tick_count;
	return ((uint8_t)((++x >> 4) + ((x << 3) & M) - (x >> 7) - ((x << 6) & M)))%range;
}

void CYZ_CMD_tick() {
	if (cyz_cmd.play_script && cyz_cmd.tick_count++ == 0) {
		uint8_t duration = CYZ_CMD_play_next_script_line();
		cyz_cmd.tick_count = -duration;
	}
}
