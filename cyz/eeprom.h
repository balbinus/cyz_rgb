#ifndef TEST_MODE
#include <avr/eeprom.h>

#define EEPROM_read_script_line(dest, lineno) \
	eeprom_busy_wait(); \
	eeprom_read_block((void*)&dest, (const void*)&EEscript.lines[lineno], 5);

#define EEPROM_read_boot_parms(dest) \
	eeprom_busy_wait(); \
	eeprom_read_block((void*)&dest, (const void*)&EEbbotp, sizeof(boot_parms));

#define EEPROM_read_addr(addr) \
	eeprom_busy_wait(); \
	addr = eeprom_read_byte(&EEaddr);

#define EEPROM_write_addr(addr) \
	eeprom_busy_wait(); \
	eeprom_write_byte(&EEaddr,addr);

#define EEPROM_write_boot_parms(src) \
	eeprom_busy_wait(); \
	eeprom_write_block( (void*)&src,(void*)&EEbbotp, sizeof(boot_parms));

#define EEPROM_write_script_line(src, lineno) \
	eeprom_busy_wait(); \
	eeprom_write_block((void*)&src,(void*)&EEscript.lines[lineno], 5);
#else
#define EEPROM_read_script_line(dest, lineno)
#define EEPROM_read_boot_parms(dest)
#define EEPROM_read_addr(addr)
#define EEPROM_write_addr(addr)
#define EEPROM_write_boot_parms(src)
#define EEPROM_write_script_line(src, lineno)
#define EEMEM

#endif
