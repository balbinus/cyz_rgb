#include "color.h"

void color_rgb_to_hsv(Color rgb, uint8_t* hue, uint8_t* sat, uint8_t* val) {
    uint8_t rgb_min, rgb_max;
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

/**
 * Ripoff of this: http://www.tecgraf.puc-rio.br/~mgattass/color/HSVtoRGB.htm
 * Modified to work with unsigned chars. Some precision is lost as intermediate values are scaled down by 16 to avoid char overflow.
 * hsv to rgb conversion theory here: http://en.wikipedia.org/wiki/HSV_color_space#Conversion_from_HSV_to_RGB
 *
 * XXX: needs deeper understanding, and a rewrite
 */
void color_hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t* r, uint8_t* g, uint8_t* b)
{
	if ( s == 0 )
	{
		*r = v;
		*g = v;
		*b = v;
	}
	else
	{
		int var_i = (h/16) / 42;
		int var_h = ((h*6)/16)%16;
		int var_1 = v/16 * ( 16 - s/16 );
		int var_2 = v/16 * ( 16 - ((s/16) * var_h)/16 );
		int var_3 = v/16 * ( 16 - ((s/16) * (16-var_h))/16 );
		if      ( var_i == 0 ) { *r = v     ; *g = var_3 ; *b = var_1; }
		else if ( var_i == 1 ) { *r = var_2 ; *g = v     ; *b = var_1; }
		else if ( var_i == 2 ) { *r = var_1 ; *g = v     ; *b = var_3; }
		else if ( var_i == 3 ) { *r = var_1 ; *g = var_2 ; *b = v;     }
		else if ( var_i == 4 ) { *r = var_3 ; *g = var_1 ; *b = v;     }
		else                   { *r = v     ; *g = var_1 ; *b = var_2; }
	}
}
