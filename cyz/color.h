#ifndef _COLOR_H_
#define _COLOR_H_
#import <inttypes.h>
typedef struct _color {
    uint8_t r;
	uint8_t g;
	uint8_t b;
} Color;

#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))

void color_rgb_to_hsv(Color rgb, uint8_t* hue, uint8_t* sat, uint8_t* val);


void color_hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t* r, uint8_t* g, uint8_t* b);


#endif
