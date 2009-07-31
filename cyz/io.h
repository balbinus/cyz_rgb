#ifndef TEST_MODE
#include <avr/io.h>
#define PWM_PORT PORTB
#define PWM_DDR DDRB

/* map leds to actual pins */
#define PINRED PB3
#define PINGRN PB4
#define PINBLU PB1

/******************************/
/* turn off single leds */
#define RED_LED_OFF PWM_PORT &= ~(1<<PINRED)
#define GRN_LED_OFF PWM_PORT &= ~(1<<PINGRN)
#define BLU_LED_OFF PWM_PORT &= ~(1<<PINBLU)

/* turn on single leds */
#define RED_LED_ON PWM_PORT |= 1<<PINRED
#define GRN_LED_ON PWM_PORT |= 1<<PINGRN
#define BLU_LED_ON PWM_PORT |= 1<<PINBLU
#else
uint8_t pin_red, pin_grn, pin_blu, portb, ddrb;
#define PWM_PORT portb
#define PWM_DDR ddrb
#define PINRED pin_red
#define PINGRN pin_grn
#define PINBLU pin_blu
#define RED_LED_OFF PWM_PORT &= ~(1<<PINRED)
#define GRN_LED_OFF PWM_PORT &= ~(1<<PINGRN)
#define BLU_LED_OFF PWM_PORT &= ~(1<<PINBLU)
#define RED_LED_ON PWM_PORT |= 1<<PINRED
#define GRN_LED_ON PWM_PORT |= 1<<PINGRN
#define BLU_LED_ON PWM_PORT |= 1<<PINBLU
#endif
