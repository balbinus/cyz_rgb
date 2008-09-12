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
/** mock macros defined when in test mode.
 * regular implementation compiles only with avr-gcc.
 * mock_buf is the destination for _write operations and source for _read ops **/
#include <string.h>
uint8_t mock_buf[6];
#define mock_empty_buffer memset(&mock_buf, 0, 6);
#define EEPROM_read_script_line(dest, lineno) \
	memcpy(&dest, &mock_buf, 5); \
	mock_buf[5] = lineno;
#define EEPROM_read_boot_parms(dest) \
	memcpy(&dest, &mock_buf, 6);
#define EEPROM_read_addr(addr)
#define EEPROM_write_addr(addr)
#define EEPROM_write_boot_parms(src) \
	mock_empty_buffer \
	memcpy(&mock_buf, &src, 5);
#define EEPROM_write_script_line(src, lineno) \
	mock_empty_buffer \
	memcpy(&mock_buf, &src, 5); \
	mock_buf[5] = lineno;
#define EEMEM

#endif
