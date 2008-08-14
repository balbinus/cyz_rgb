#include "cyz_cmd.h"

Cyz_cmd* CYZ_CMD_GET_INSTANCE(Cyz_rgb* cyz_rgb) {
	 Cyz_cmd* instance = (Cyz_cmd*) malloc(sizeof(struct CYZ_CMD));
	 instance->cyz_rgb = cyz_rgb;
	 instance->execute = (void*)_CYZ_CMD_execute;
	 instance->receive_one_byte = (void*)_CYZ_CMD_receive_one_byte;
	 instance->play_next_script_line = (void*)_CYZ_CMD_play_next_script_line;
	 instance->play_script = 0;
	 instance->script_end = 0;
	 instance->script_pos = 0;
	 instance->script_repeats = 0;
	 instance->script_repeated = 0;
	 return instance;
}

/* returns the length of the command, command+payload. */
/* example "fadeToColor" is 'f',R,G,B: the returned length is 4 */
/* 0xFF means error */
unsigned char CYZ_CMF_get_cmd_len (char cmd) {
	switch(cmd) {
		case CMD_GO_TO_RGB:
			return 4;
		case CMD_FADE_TO_RGB:
			return 4;
		case CMD_WRITE_SCRIPT_LINE:
			return 8;
		case CMD_PLAY_LIGHT_SCRIPT:
			return 3;
	}
	return 0xFF;
}


void _CYZ_CMD_execute(Cyz_cmd *this, unsigned char* cmd) {
	switch (cmd[0]) {
		case CMD_GO_TO_RGB:
			this->cyz_rgb->set_color(this->cyz_rgb, cmd[1], cmd[2] , cmd[3]);
		break;
		case CMD_FADE_TO_RGB:
			this->cyz_rgb->set_fade_color(this->cyz_rgb, cmd[1], cmd[2], cmd[3]);
		break;
		case CMD_WRITE_SCRIPT_LINE:
		{
			//TODO: cmd[1] (script id) is ignored, only one script can be written
			script_line* line = (script_line*) malloc(sizeof(struct _script_line));
			line->dur = cmd[3];
			line->cmd[0] = cmd[4];
			line->cmd[1] = cmd[5];
			line->cmd[2] = cmd[6];
			line->cmd[3] = cmd[7];
			this->script[cmd[2]] = line;

			if (cmd[2] > this->script_end) {
				this->script_end = cmd[2];
			}
		}
		break;
		case CMD_PLAY_LIGHT_SCRIPT:
			this->play_script = 1;
			//cmd[1] is script number, currently ignore, we only play script 0
			this->script_repeats = cmd[2];
			this->script_pos = cmd[3];
		break;
	}
}

/* to be invoked inside a timer, every time its called plays next script line, */
/* if script is playing and there are more lines to play */
void _CYZ_CMD_play_next_script_line(Cyz_cmd *this) {
	if (this->play_script == 1) {
		this->execute(this, this->script[this->script_pos++]->cmd);
		if (this->script_pos > this->script_end) {
			this->script_pos = 0;
			this->script_repeated++;
			if (this->script_repeats > 0 && this->script_repeated >= this->script_repeats){
				this->play_script = 0;
				this->script_repeated = 0;
			}
		}
	}
}

void _CYZ_CMD_receive_one_byte(Cyz_cmd *this, unsigned char in) {
	this->rcv_cmd_buf[this->rcv_cmd_buf_cnt] = in;
	/* first byte contains the command, use it to decide length of payload */
	if (this->rcv_cmd_buf_cnt == 0) {
		this->rcv_cmd_len = CYZ_CMF_get_cmd_len(this->rcv_cmd_buf[this->rcv_cmd_buf_cnt]);
	}

	if (this->rcv_cmd_len <= 8 && this->rcv_cmd_buf_cnt==(this->rcv_cmd_len-1)) {
		this->execute(this, this->rcv_cmd_buf);
		this->rcv_cmd_buf_cnt = 0;
	}
	else {
		this->rcv_cmd_buf_cnt++;
	}
}
