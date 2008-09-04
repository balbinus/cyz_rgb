#ifndef CYZ_CMD_H
#define CYZ_CMD_H
/**
 * Command parser and executor
 *
 */

#include "cyz_rgb.h"

#define CYZ_CMD_MAX_CMD_LEN 8
#define CYZ_CMD_BOOTP_MAGIC 33
#define CMD_GO_TO_RGB 'n'
#define CMD_FADE_TO_RGB 'c'
#define CMD_WRITE_SCRIPT_LINE 'W'
#define CMD_PLAY_LIGHT_SCRIPT 'p'
#define CMD_STOP_SCRIPT 'o'
#define CMD_SET_BOOT_PARMS 'B'
#define CMD_SET_FADESPEED 'f'
#define CMD_SET_TIMEADJUST 't'
#define CMD_SET_LEN_RPTS 'L'

#define MAX_SCRIPT_LEN 10

typedef struct _boot_parms {
	uint8_t magic; // check byte to see if any data has been stored
	uint8_t mode; // 0 do nothing, 1 play light script
	uint8_t scriptno; // which script to play if mode==1
	uint8_t repeats; // number of repetitions
	uint8_t fadespeed;
	int8_t timeadjust;
} boot_parms;

typedef struct _script_line {
    uint8_t dur;
    uint8_t cmd[4];    // cmd,arg1,arg2,arg3
} script_line;

typedef struct CYZ_CMD {
	script_line script[MAX_SCRIPT_LEN];
	Cyz_rgb* cyz_rgb;
	uint8_t rcv_cmd_buf[8];
	uint8_t rcv_cmd_buf_cnt;
	uint8_t rcv_cmd_len;
	uint8_t play_script;
	uint8_t script_length;
	uint8_t script_pos;
	uint8_t script_repeats;
	uint8_t script_repeated;
	uint8_t timeadjust;
} Cyz_cmd;



void CYZ_CMD_GET_INSTANCE(Cyz_rgb* cyz_rgb);
void _CYZ_CMD_execute(uint8_t* cmd);
void _CYZ_CMD_receive_one_byte(uint8_t in);
long _CYZ_CMD_play_next_script_line();
void CYZ_CMD_load_boot_params();

#endif
