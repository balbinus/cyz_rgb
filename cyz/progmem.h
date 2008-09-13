#ifndef TEST_MODE
#include <avr/pgmspace.h>

#define PROGMEM_read_script_line(dst, scriptno, lineno) \
	script* PGMscr = ((script*)pgm_read_word(&scripts[scriptno])); \
	cyz_cmd.script_length = pgm_read_byte(PGMscr);\
	memcpy_P(&dst, &PGMscr->lines[lineno], 5);
#else
uint8_t progmem_mock_buf[7];
#define PROGMEM
#define PROGMEM_read_script_line(dest, scriptno, lineno) \
memcpy(&dest, &progmem_mock_buf, 5); \
progmem_mock_buf[5] = lineno; \
progmem_mock_buf[6] = scriptno;
#endif
