#ifndef TEST_MODE
#include <avr/pgmspace.h>

#define PROGMEM_read_script_line(dst, scriptno, lineno) \
	script* PGMscr = ((script*)pgm_read_word(&scripts[scriptno])); \
	cyz_cmd.script_length = pgm_read_byte(PGMscr);\
	memcpy_P(&dst, &PGMscr->lines[lineno], 5);
#else
	#define PROGMEM_read_script_line(dst, scriptno, lineno)
	#define PROGMEM
#endif
