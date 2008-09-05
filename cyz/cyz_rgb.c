#include "cyz_rgb.h"

Cyz_rgb cyz_rgb;

Cyz_rgb* _CYZ_RGB_get() {
	return &cyz_rgb;
}

void _CYZ_RGB_set_color(uint8_t r, uint8_t g, uint8_t b) {
	cyz_rgb.fade = 0;
	cyz_rgb.color.r = r;
	cyz_rgb.color.g = g;
	cyz_rgb.color.b = b;
}

void _CYZ_RGB_set_fade_color(uint8_t r, uint8_t g, uint8_t b) {
	cyz_rgb.fade = 1;
	cyz_rgb.fade_color.r = r;
	cyz_rgb.fade_color.g = g;
	cyz_rgb.fade_color.b = b;
}

/**
 * Ripoff of this: http://www.tecgraf.puc-rio.br/~mgattass/color/HSVtoRGB.htm
 * Modified to work with unsigned chars. Some precision is lost as intermediate values are scaled down by 16 to avoid char overflow.
 * hsv to rgb conversion theory here: http://en.wikipedia.org/wiki/HSV_color_space#Conversion_from_HSV_to_RGB
 *
 * XXX: needs deeper understanding, and a rewrite
 */
void _CYZ_RGB_set_fade_color_hsb(uint8_t h, uint8_t s, uint8_t v)
{
	if ( s == 0 )
	{
		cyz_rgb.fade_color.r = v;
		cyz_rgb.fade_color.g = v;
		cyz_rgb.fade_color.b = v;
	}
	else
	{
		int var_i = (h/16) / 42;
		int var_h = ((h*6)/16)%16;
		int var_1 = v/16 * ( 16 - s/16 );
		int var_2 = v/16 * ( 16 - ((s/16) * var_h)/16 );
		int var_3 = v/16 * ( 16 - ((s/16) * (16-var_h))/16 );
		if      ( var_i == 0 ) { cyz_rgb.fade_color.r = v     ; cyz_rgb.fade_color.g = var_3 ; cyz_rgb.fade_color.b = var_1; }
		else if ( var_i == 1 ) { cyz_rgb.fade_color.r = var_2 ; cyz_rgb.fade_color.g = v     ; cyz_rgb.fade_color.b = var_1; }
		else if ( var_i == 2 ) { cyz_rgb.fade_color.r = var_1 ; cyz_rgb.fade_color.g = v     ; cyz_rgb.fade_color.b = var_3; }
		else if ( var_i == 3 ) { cyz_rgb.fade_color.r = var_1 ; cyz_rgb.fade_color.g = var_2 ; cyz_rgb.fade_color.b = v;     }
		else if ( var_i == 4 ) { cyz_rgb.fade_color.r = var_3 ; cyz_rgb.fade_color.g = var_1 ; cyz_rgb.fade_color.b = v;     }
		else                   { cyz_rgb.fade_color.r = v     ; cyz_rgb.fade_color.g = var_1 ; cyz_rgb.fade_color.b = var_2; }
	}
}
#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))

void _CYZ_RGB_rgb_to_hsv(Color rgb, uint8_t* hue, uint8_t* sat, uint8_t* val) {
    unsigned char rgb_min, rgb_max;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    *val = rgb_max;
    if (*val == 0) {
        *hue = *sat = 0;
        return;
    }

    *sat = 255*(rgb_max - rgb_min)/ *val;
    if (*sat == 0) {
        *hue = 0;
        return;
    }

    /* Compute hue */
    if (rgb_max == rgb.r) {
        *hue = 0 + 43*(rgb.g - rgb.b)/(rgb_max - rgb_min);
    } else if (rgb_max == rgb.g) {
        *hue = 85 + 43*(rgb.b - rgb.r)/(rgb_max - rgb_min);
    } else /* rgb_max == rgb.b */ {
        *hue = 171 + 43*(rgb.r - rgb.g)/(rgb_max - rgb_min);
    }
}

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

void __CYZ_RGB_fade_step() {
	if (cyz_rgb.color.r != cyz_rgb.fade_color.r) {
		if (cyz_rgb.color.r > cyz_rgb.fade_color.r) {
			uint8_t diff = cyz_rgb.color.r - cyz_rgb.fade_color.r;
			cyz_rgb.color.r -= MIN(diff, cyz_rgb.fadespeed);
		}
		else {
			uint8_t diff = cyz_rgb.fade_color.r - cyz_rgb.color.r;
			cyz_rgb.color.r += MIN(diff, cyz_rgb.fadespeed);
		}
	}
	if (cyz_rgb.color.g != cyz_rgb.fade_color.g) {
		if (cyz_rgb.color.g > cyz_rgb.fade_color.g) {
			uint8_t diff = cyz_rgb.color.g - cyz_rgb.fade_color.g;
			cyz_rgb.color.g -= MIN(diff, cyz_rgb.fadespeed);
		}
		else {
			uint8_t diff = cyz_rgb.fade_color.g - cyz_rgb.color.g;
			cyz_rgb.color.g += MIN(diff, cyz_rgb.fadespeed);
		}
	}
	if (cyz_rgb.color.b != cyz_rgb.fade_color.b) {
		if (cyz_rgb.color.b > cyz_rgb.fade_color.b) {
			uint8_t diff = cyz_rgb.color.b - cyz_rgb.fade_color.b;
			cyz_rgb.color.b -= MIN(diff, cyz_rgb.fadespeed);
		}
		else {
			uint8_t diff = cyz_rgb.fade_color.b - cyz_rgb.color.b;
			cyz_rgb.color.b += MIN(diff, cyz_rgb.fadespeed);
		}
	}

	//TODO: set cyz_rgb.fade=0  when fade_color is reached
}

void _CYZ_RGB_pulse() {
	if (++cyz_rgb.pulse_count == 0) {
		if (cyz_rgb.color.r > 0) RED_LED_ON;
		if (cyz_rgb.color.g > 0) GRN_LED_ON;
		if (cyz_rgb.color.b > 0) BLU_LED_ON;
		if(cyz_rgb.fade==1) {
			__CYZ_RGB_fade_step();
		}
	}
	if (cyz_rgb.color.r != 255 && cyz_rgb.pulse_count == cyz_rgb.color.r) RED_LED_OFF;
	if (cyz_rgb.color.g != 255 && cyz_rgb.pulse_count == cyz_rgb.color.g) GRN_LED_OFF;
	if (cyz_rgb.color.b != 255 && cyz_rgb.pulse_count == cyz_rgb.color.b) BLU_LED_OFF;
}

Cyz_rgb* CYZ_RGB_GET_INSTANCE() {
	cyz_rgb.pulse_count = 0xFF;
	cyz_rgb.fade = 0;
	cyz_rgb.fadespeed = 1;
	PWM_DDR |= 1<<PINRED;
	PWM_DDR |= 1<<PINGRN;
	PWM_DDR |= 1<<PINBLU;
	return &cyz_rgb;
}
