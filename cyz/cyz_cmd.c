#include "cyz_cmd.h"

Cyz_cmd* CYZ_CMD_GET_INSTANCE(Cyz_rgb* cyz_rgb) {
	 Cyz_cmd* instance = (Cyz_cmd*) malloc(sizeof(struct CYZ_CMD));
	 instance->cyz_rgb = cyz_rgb;
	 instance->execute = (void*)_CYZ_CMD_execute;
	 instance->receive_one_byte = (void*)_CYZ_CMD_receive_one_byte;
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
