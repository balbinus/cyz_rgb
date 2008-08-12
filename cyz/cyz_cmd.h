#define CYZ_CMD_MAX_CMD_LEN 8;

#define CMD_GO_TO_RGB 'n'
#define CMD_FADE_TO_RGB 'c'

unsigned char CYZ_CMD_rcv_buf[8];
unsigned char CYZ_CMD_rcv_cnt = 0;
unsigned char CYZ_CMD_current_cmd_len = 0xFF;

/* returns the legth of the command, including the command defintion */
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

typedef struct _script_line {
    unsigned char dur;
    unsigned char cmd[4];    // cmd,arg1,arg2,arg3
} script_line;

script_line simple_script[2];

void CYZ_CMD_play_script_line(Cyz_rgb *cyz_rgb, script_line* line) {
	cyz_rgb->fade = 1;
	cyz_rgb->fade_color.r = line->cmd[1];
	cyz_rgb->fade_color.g = line->cmd[2];
	cyz_rgb->fade_color.b = line->cmd[3];
}

void CYZ_CMD_receive_and_execute(Cyz_rgb *cyz_rgb, unsigned char in) {
	CYZ_CMD_rcv_buf[CYZ_CMD_rcv_cnt] = in;
	/* first byte contains the command, use it to decide length of payload */
	if (CYZ_CMD_rcv_cnt == 0) {
		CYZ_CMD_current_cmd_len = CYZ_CMF_get_cmd_len(CYZ_CMD_rcv_buf[CYZ_CMD_rcv_cnt]);
	}

	if (CYZ_CMD_current_cmd_len <= 8 && CYZ_CMD_rcv_cnt==CYZ_CMD_current_cmd_len) {
		switch (CYZ_CMD_rcv_buf[0]) {
		case CMD_GO_TO_RGB:
			cyz_rgb->set_color(cyz_rgb, CYZ_CMD_rcv_buf[1], CYZ_CMD_rcv_buf[2] , CYZ_CMD_rcv_buf[3]);
		break;
		case CMD_FADE_TO_RGB:
			cyz_rgb->set_fade_color(cyz_rgb, CYZ_CMD_rcv_buf[1],CYZ_CMD_rcv_buf[2],CYZ_CMD_rcv_buf[3]);
		break;
		}
		CYZ_CMD_rcv_cnt = 0;
	}
	else {
		CYZ_CMD_rcv_cnt++;
	}

}
