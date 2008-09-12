#ifndef TEST_MODE
#include <avr/pgmspace.h>

#define PROGMEM_read_script_line(dst, scriptno, lineno) \
	script* PGMscr = ((script*)pgm_read_word(&scripts[scriptno])); \
	cyz_cmd.script_length = pgm_read_byte(PGMscr);\
	memcpy_P(&dst, &PGMscr->lines[lineno], 5);
#else
uint8_t mock_buf[6];
#define mock_empty_buffer memset(&mock_buf, 0, 6);
#define PROGMEM
#define PROGMEM_read_script_line(dest, scriptno, lineno) \
memcpy(&dest, &mock_buf, 5); \
mock_buf[5] = lineno;
#endif
