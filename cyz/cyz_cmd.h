#ifndef _CYZ_CMD_H_
#define _CYZ_CMD_H_
/**
 * Command parser and executor
 *
 */
#include <inttypes.h>
#include "color.h"
#include "eeprom.h"
#include "progmem.h"
#include "../ring_buffer.h"

#define VERSION_MAJOR 0x01
#define VERSION_MINOR 0x03

#define CYZ_CMD_MAX_CMD_LEN 8
#define CYZ_CMD_BOOTP_MAGIC 33
#define CMD_GO_TO_RGB 'n'
#define CMD_FADE_TO_RGB 'c'
#define CMD_FADE_TO_RND_RGB 'C'
#define CMD_FADE_TO_HSB 'h'
#define CMD_FADE_TO_RND_HSB 'H'
#define CMD_WRITE_SCRIPT_LINE 'W'
#define CMD_PLAY_LIGHT_SCRIPT 'p'
#define CMD_STOP_SCRIPT 'o'
#define CMD_SET_BOOT_PARMS 'B'
#define CMD_SET_FADESPEED 'f'
#define CMD_SET_TIMEADJUST 't'
#define CMD_SET_LEN_RPTS 'L'
#define CMD_SET_ADDR 'A'
#define CMD_GET_ADDR 'a'
#define CMD_GET_RGB 'g'
#define CMD_GET_SCRIPT_LINE 'R'
#define CMD_GET_FIRMWARE_VERSION 'Z'

#define CMD_GET_DBG 'D'

#define MAX_SCRIPT_LEN 10
extern Color led_curr_color;
extern Color led_fade_color;
extern uint8_t led_fade;
extern uint8_t led_fadespeed;

typedef struct _boot_parms {
	uint8_t magic; // check byte to see if any data has been stored
	uint8_t play_script; // 0 do nothing, 1 play light script
	uint8_t repeats; // number of repetitions
	uint8_t fadespeed;
	int8_t timeadjust;
} boot_parms;

typedef struct _script_line {
    uint8_t dur;
    uint8_t cmd[4];    // cmd,arg1,arg2,arg3
} script_line;

typedef struct _script {
    uint8_t len;  // number of script lines, 0 == blank script, not playing
    uint8_t reps; // number of times to repeat, 0 == infinite playes
    script_line lines[];
} script;

typedef struct CYZ_CMD {
	uint8_t rcv_cmd_buf[8];
	uint8_t rcv_cmd_buf_cnt;
	uint8_t rcv_cmd_len;
	uint8_t play_script;
	uint8_t script_length;
	uint8_t script_pos;
	uint8_t script_repeats;
	uint8_t script_repeated;
	uint8_t timeadjust;
	uint8_t addr;
	uint8_t tick_count;
	ring_buffer send_buffer;
	uint8_t dbg[8];
} Cyz_cmd;

Cyz_cmd cyz_cmd;

void CYZ_CMD_init();
void _CYZ_CMD_execute(uint8_t* cmd);
void CYZ_CMD_receive_one_byte(uint8_t in);
void CYZ_CMD_load_boot_params();
void CYZ_CMD_tick();
uint8_t CYZ_CMD_play_next_script_line();
uint8_t CYZ_CMD_prng(uint8_t range);
uint8_t CYZ_CMD_get_cmd_len (uint8_t cmd);

#endif
