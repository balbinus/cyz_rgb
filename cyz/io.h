#ifndef TEST_MODE
#include <avr/io.h>
#include "cyz_rgb.h"

/* BlinkM */
#if defined( BLINKM_TYPE_BLINKM )
#  define PWM_PORT_R PORTB
#  define PWM_PORT_G PORTB
#  define PWM_PORT_B PORTB
#  define PWM_DDR_R DDRB
#  define PWM_DDR_G DDRB
#  define PWM_DDR_B DDRB
  /* map leds to actual pins */
#  define PINRED PB3
#  define PINGRN PB4
#  define PINBLU PB1
#endif

#if defined( BLINKM_TYPE_MAXM )
#  define PWM_PORT_R PORTB
#  define PWM_PORT_G PORTA
#  define PWM_PORT_B PORTA
#  define PWM_DDR_R DDRB
#  define PWM_DDR_G DDRA
#  define PWM_DDR_B DDRA
  /* map leds to actual pins */
#  define PINRED PB2
#  define PINGRN PA7
#  define PINBLU PA5
#endif

/******************************/
/* turn off single leds */
#define RED_LED_OFF PWM_PORT_R &= ~(1<<PINRED)
#define GRN_LED_OFF PWM_PORT_G &= ~(1<<PINGRN)
#define BLU_LED_OFF PWM_PORT_B &= ~(1<<PINBLU)

/* turn on single leds */
#define RED_LED_ON PWM_PORT_R |= 1<<PINRED
#define GRN_LED_ON PWM_PORT_G |= 1<<PINGRN
#define BLU_LED_ON PWM_PORT_B |= 1<<PINBLU
#else
uint8_t pin_red, pin_grn, pin_blu, portb, ddrb;
#define PWM_PORT_R portb
#define PWM_PORT_G portb
#define PWM_PORT_B portb
#define PWM_DDR_R ddrb
#define PWM_DDR_G ddrb
#define PWM_DDR_B ddrb
#define PINRED pin_red
#define PINGRN pin_grn
#define PINBLU pin_blu
#define RED_LED_OFF PWM_PORT_R &= ~(1<<PINRED)
#define GRN_LED_OFF PWM_PORT_G &= ~(1<<PINGRN)
#define BLU_LED_OFF PWM_PORT_B &= ~(1<<PINBLU)
#define RED_LED_ON PWM_PORT_R |= 1<<PINRED
#define GRN_LED_ON PWM_PORT_G |= 1<<PINGRN
#define BLU_LED_ON PWM_PORT_B |= 1<<PINBLU
#endif
