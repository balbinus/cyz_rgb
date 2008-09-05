#include "cyz_cmd.h"
#include <avr/eeprom.h>
#include <limits.h>
#include "cyz_rgb.h"

boot_parms EEMEM EEbbotp;
script_line EEMEM EEscript[MAX_SCRIPT_LEN];
uint8_t EEMEM EEaddr;
Cyz_cmd cyz_cmd;

void CYZ_CMD_GET_INSTANCE(Cyz_rgb* cyz_rgb) {
	cyz_cmd.cyz_rgb = cyz_rgb;
	cyz_cmd.rcv_cmd_buf_cnt = 0;
	cyz_cmd.rcv_cmd_len = 0xFF;
	cyz_cmd.play_script = 0;
	cyz_cmd.script_length = 0;
	cyz_cmd.script_pos = 0;
	cyz_cmd.script_repeats = 0;
	cyz_cmd.script_repeated = 0;
	cyz_cmd.timeadjust = 0;
	cyz_cmd.addr = 0x0d;
	cyz_cmd.tick_count = 1;
}

/* returns the length of the command, command+payload. */
/* example "fadeToColor" is 'f',R,G,B: the returned length is 4 */
/* 0xFF means error */
uint8_t CYZ_CMD_get_cmd_len (char cmd) {
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
		return 1;
	case CMD_SET_FADESPEED:
		return 1;
	case CMD_SET_LEN_RPTS:
		return 4;
	case CMD_SET_ADDR:
		return 5;
	}
	return 0xFF;
}

void _CYZ_CMD_execute(uint8_t* cmd) {
	switch (cmd[0]) {
	case CMD_GO_TO_RGB:
		_CYZ_RGB_set_color(cmd[1],cmd[2],cmd[3]);
		break;
	case CMD_FADE_TO_RGB:
		_CYZ_RGB_set_fade_color(cmd[1],cmd[2],cmd[3]);
		break;
	case CMD_FADE_TO_RND_RGB:
		_CYZ_RGB_set_fade_color(
			cyz_cmd.cyz_rgb->color.r + _CYZ_CMD_prng(cmd[1]),
			cyz_cmd.cyz_rgb->color.g + _CYZ_CMD_prng(cmd[2]),
			cyz_cmd.cyz_rgb->color.b + _CYZ_CMD_prng(cmd[3]));
		break;
	case CMD_FADE_TO_HSB:
		_CYZ_RGB_set_fade_color_hsb(cmd[1], cmd[2], cmd[3]);
		break;
	case CMD_FADE_TO_RND_HSB:
		break;
	case CMD_WRITE_SCRIPT_LINE:
	{
		if (cmd[2] > MAX_SCRIPT_LEN-1)
			break;
		//TODO: cmd[1] (script id) is ignored, only one script can be written
		// cmd[2] is line number
		script_line tmp;
		tmp.dur = cmd[3];
		tmp.cmd[0] = cmd[4];
		tmp.cmd[1] = cmd[5];
		tmp.cmd[2] = cmd[6];
		tmp.cmd[3] = cmd[7];

		eeprom_busy_wait();
		eeprom_write_block((void*)&tmp,(void*)&EEscript[cmd[2]], 5);
	}
	break;
	case CMD_PLAY_LIGHT_SCRIPT:
		//cmd[1] is script number, currently ignore, we only play script 0
		cyz_cmd.script_repeats = cmd[2];
		cyz_cmd.script_pos = cmd[3];
		cyz_cmd.play_script = 1;
		break;
	case CMD_STOP_SCRIPT:
		cyz_cmd.play_script = 0;
		break;
	case CMD_SET_BOOT_PARMS:
	{
		boot_parms temp;
		temp.magic = CYZ_CMD_BOOTP_MAGIC;
		temp.mode = cmd[1];
		temp.repeats = cmd[2];
		temp.scriptno = cmd[3];
		temp.fadespeed = cmd[4];
		temp.timeadjust = cmd[5];
		eeprom_busy_wait();
		eeprom_write_block( (void*)&temp,(void*)&EEbbotp, sizeof(boot_parms));
	}
	case CMD_SET_FADESPEED:
		if (cmd[1] != 0) {
			cyz_cmd.cyz_rgb->fadespeed = cmd[1];
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
		if (cmd[2] == 0x0d && cmd[3] == 0x0d && cmd[1] == cmd[2]) {
			cyz_cmd.addr = cmd[1];
			eeprom_write_byte(&EEaddr,cyz_cmd.addr);
		}
		break;
	}
}

void CYZ_CMD_load_boot_params() {
	eeprom_busy_wait();
	cyz_cmd.addr = eeprom_read_byte(&EEaddr);

	boot_parms temp;
	eeprom_busy_wait();
	eeprom_read_block((void*)&temp, (const void*)&EEbbotp, sizeof(boot_parms));
	if (temp.magic == CYZ_CMD_BOOTP_MAGIC) {
		if (temp.mode == 1) {
			cyz_cmd.play_script = temp.mode;
			cyz_cmd.script_repeats = temp.repeats;
			if (temp.fadespeed != 0) {
				cyz_cmd.cyz_rgb->fadespeed = temp.fadespeed;
			}
			cyz_cmd.timeadjust = temp.timeadjust;
			//cyz_cmd.sciptno = temp.scriptno;
		}
	}
}

/* to be invoked inside a timer, every time its called plays next script line, */
/* if script is playing and there are more lines to play */
long _CYZ_CMD_play_next_script_line() {
	if (cyz_cmd.play_script == 1) {
		//TODO: load lines in memory only once
		script_line tmp;
		eeprom_busy_wait();
		eeprom_read_block((void*)&tmp, (const void*)&EEscript[cyz_cmd.script_pos++], 5);
		_CYZ_CMD_execute(tmp.cmd);
		if (cyz_cmd.script_pos == cyz_cmd.script_length) {
			cyz_cmd.script_pos = 0;
			cyz_cmd.script_repeated++;
			if (cyz_cmd.script_repeats > 0 && cyz_cmd.script_repeated >= cyz_cmd.script_repeats){
				cyz_cmd.play_script = 0;
				cyz_cmd.script_repeated = 0;
			}
		}

		return tmp.dur + cyz_cmd.timeadjust;
	}
	return -1;
}

void _CYZ_CMD_receive_one_byte(uint8_t in) {
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
uint8_t _CYZ_CMD_prng(uint8_t range) {
	static uint8_t count = 1;
	if (range == 0) return 0;
	uint8_t x = ++count + cyz_cmd.tick_count;
	return ((++x >> 4) + ((x << 3) & M) - (x >> 7) - ((x << 6) & M))%range;
}

void _CYZ_CMD_tick() {
	if (--cyz_cmd.tick_count == 0) {
		long duration = _CYZ_CMD_play_next_script_line();
		if (duration == -1)
			cyz_cmd.tick_count = UINT_MAX;
		else {
			cyz_cmd.tick_count = duration * 255;
		}
	}
}
