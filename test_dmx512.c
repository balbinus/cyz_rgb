/*
From: http://forum.arduino.cc/index.php?topic=278252.0

ATTINY13 - DMX512-Receiver to 4 x WS2812 driver - 9.6mhz - 962 bytes
LED to show that DMX data is being received
Up/Down buttons to select DMX RX channel - both held will reset channel to 1
Channel saved to EEprom
Requires 75176 differential transceiver chip to interface DMX to ATTiny13

Get tiny13 cores -> http://forum.arduino.cc/index.php?topic=89781.0

boards.txt entries : 
attiny13at9.name=ATtiny13 @ 9.6MHz (internal 9.6 MHz clock)
attiny13at9.bootloader.low_fuses=0x3a
attiny13at9.bootloader.high_fuses=0xff
attiny13at9.upload.maximum_size=1024
attiny13at9.build.mcu=attiny13
attiny13at9.build.f_cpu=1200000
attiny13at9.build.core=core13

                 RESET 1 -     - 8 VCC
     DEBUG/LED/A3/Pin3 2 -     - 7 Pin2/A1/SCK/WS2812 data
  DMXin+/DMXin/A2/Pin4 3 -     - 6 Pin1/MISO/PWM/CHANUP button to 0v
                   GND 4 -     - 5 Pin0/MOSI/PWM/CHANDN button to 0v
*/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define WAIT_FOR_BREAK_START 0
#define WAIT_FOR_START_BIT 1

// Pin definitions
#define DMX_IN 4 // PortB1 = actual pin 3 on chip
#define INPUT_PULLUPS 0b00000011 // pin 0 & 1 for pull ups only
#define UP_KEY 0b00000010
#define DN_KEY 0b00000001
#define KEYS   0b00000011
#define DEBUG_PIN 0b10000000 
#define LED_MASK 0b00001000 
// precise timings etc
#define MAXCHANNELS 512
#define PINMASK 0b00010000
#define DEBOUNCE 100 // debounce time in micros 
// WS2812 specific
#define DIGITAL_PIN   (2)         // Digital port number
#define PORT          (PORTB)     // Digital pin's port
#define PORT_PIN      (PORTB2)    // Digital pin's bit position
#define DMX_PIN       (PORTB1)
#define NUM_CHANS     (13) 

uint8_t lastRaw, key; // 2 bytes (2) 
uint8_t rxData, state, chCount, x, LED_count; // 5 bytes (7) // flags,
unsigned long keyTime; // 4 bytes (11)
uint16_t DMXstart, rxCount; // 4 bytes (15)
uint8_t dmxData[NUM_CHANS];  

void EEPROMwrite(uint16_t val);

int main(void)
{
    // Init
    DDRB |= 0b00001100; // set data direction for debug etc
    PORTB |= INPUT_PULLUPS; // used when setting key inputs
    // EEPROM INIT (if uninitialsed, then init !)
    if (eeprom_read_byte((unsigned char *) 2))
    {
        eeprom_write_byte((unsigned char *) 2, 0); // "canary"
        EEPROMwrite(1); 
    }
    // READ EEPROM
    DMXstart = eeprom_read_byte((unsigned char *) 0) + (eeprom_read_byte((unsigned char *) 1) << 8); 
    cli();
    
    // Run forever
    for (;;)
    {
        cli();
        switch (state)
        {
            case WAIT_FOR_BREAK_START:
                asm ("L_%=:\n\t" // %= outputs a number that is unique to each instance of the asm statement in the entire compilation
                       "sbic %0, %1\n\t" // SBIC - Skip (next instruction) if Bit in I/O Register is Cleared
                       "rjmp L_%=\n\t"
                      :: "I" (_SFR_IO_ADDR(PORT)), "I" (DMX_PIN)  ); // wait for break to start (HIGH->LOW) 
                chCount = 0;
                // this value should really be <22 to measure 88us
                while (chCount < 22)
                {
                    //~ asm ( "ldi %0,8\n\t"
                       //~ "break:\n\t"
                          //~ "dec %0\n\t"
                          //~ "brne break\n"
                          //~ :: "r" (8) ); // to give a 4us loop 
                    _delay_us(4); // 4 us delay
                    if (PINB & PINMASK)
                    {
                        // if High then reset count
                        chCount = 0;
                    }
                    else
                    {
                        chCount++;
                    } 
                }
                while (!(PINB & PINMASK)) { }  // wait for break to expire 
                
                // heartbeat indicator 
                if (!LED_count--)
                {
                    PORTB ^= LED_MASK;
                    LED_count = 16;
                }
                rxCount = chCount = 0; // reset byte counter
                state = WAIT_FOR_START_BIT; // set next state

            case WAIT_FOR_START_BIT:
                while (PINB & PINMASK) { } // wait while input is high
                uint8_t m = 1; // start bit mask at 0b00000001
                asm ("here1:\n dec %0\n brne here1\n" :: "r" (5) ); // delay to align sample of first bit
                
                for (x = 0; x < 8; x++)
                {
                    asm ("ldi %0,7\n nop\n nop\n bits:\n dec %0\n brne bits\n" :: "r" (7) ); 
                    // the above line may require adjustment for internal resonator
                    // generally the 2 x NOPs work, but I have seen some requiring either one NOP or even NONE
                    PORTB |= DEBUG_PIN; // do not remove
                    if (PINB & PINMASK) { rxData |= m; }  
                    if (!(PINB & PINMASK)) { rxData &= ~m; }  
                    m <<= 1; // shift mask right
                    PORTB &= ~DEBUG_PIN; // do not remove
                }
                asm ("stop:\n dec %0\n brne stop\n" :: "r" (5) ); // post bit read delay, wait for stop bits

                // stop bit found ?
                if ((PINB & PINMASK) && (chCount <= NUM_CHANS))
                {
                    if (rxCount >= DMXstart) { dmxData[++chCount] = rxData; } // ensure captured channel is in our range
                    rxCount++; // get next byte
                    state = WAIT_FOR_START_BIT; // go and grab next byte
                    break;
                }
                // if you get to here then all required channels have been captured, so dump rest of packet and do WS2812 stuff
                else
                {
                    uint8_t t;
                    for (x = 1 ; x < NUM_CHANS ; x += 3)
                    { 
                        t=dmxData[x+1]; dmxData[x+1]=dmxData[x]; dmxData[x]=t; // rearrange RGB input to GRB for WS2812
                    }
                    
                    // --- FIXME FIXME output data to devices
                    
                    state = WAIT_FOR_BREAK_START; // start all over again
                    break;
                }
        }
    }
    
    return 0;
}

void EEPROMwrite(uint16_t val)
{ 
    eeprom_write_byte((unsigned char *) 0, val); 
    eeprom_write_byte((unsigned char *) 1, val>>8); 
}
