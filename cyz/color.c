
#include "color.h"

/* TODO This function is only used by the fade to random HSV. It might be possible to eliminate */
void color_rgb_to_hsv(Color rgb, uint8_t* hue, uint8_t* sat, uint8_t* val) {

    uint8_t rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    uint8_t rgb_max = MAX3(rgb.r, rgb.g, rgb.b);
	uint8_t rgb_delta = rgb_max - rgb_min;

    *val = rgb_max;
    if (*val == 0) {
        *hue = *sat = 0;
        return;
    }

    *sat = 255*(rgb_delta)/ *val;
    if (*sat == 0) {
        *hue = 0;
        return;
    }

    // Compute hue 
    if (rgb_max == rgb.r) {
        *hue = 0 + 43*(rgb.g - rgb.b)/rgb_delta;
    } else if (rgb_max == rgb.g) {
        *hue = 85 + 43*(rgb.b - rgb.r)/rgb_delta;
    } else { // rgb_max == rgb.b
        *hue = 171 + 43*(rgb.r - rgb.g)/rgb_delta;
    }

}

/**
 * Modified algorith at http://www.tecgraf.puc-rio.br/~mgattass/color/HSVtoRGB.htm to work with uint8_t
 * Some precision is lost as intermediate values are scaled down by 16 to avoid 8 bit overflow.
 * HSV to RGB conversion theory here: http://en.wikipedia.org/wiki/HSV_color_space#Conversion_from_HSV_to_RGB
 */
void color_hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t* r, uint8_t* g, uint8_t* b)
{

	if ( s == 0 )
	{
		*b = *g = *r = v;
	}
	else
	{	
		
		uint8_t var_i = h/43;                                 // Hue quadrant (sixths) - 43 is (256/6)
		uint8_t var_h = (var_i*43)/16;                        // Caculate quadrant floor/16: 
		                                                      // H Input of 0..255 should produce 0, 2, 5, 8, 10, 13
		s /= 16;                                              // Pre-scale S
		uint8_t var_v = v/16;                                 // Pre-scale V
		                                                      // These interpolations using uint8 range 0..255 are equivalent
		                                                      // to using the below algorithm with floating point range 0..1
		uint8_t var_1 = var_v * ( 16 - s );                   // V*(1-S)
		uint8_t var_2 = var_v * ( 16 - (s * var_h)/16 );      // V*(1-(S*H_Floor))
		uint8_t var_3 = var_v * ( 16 - (s * (16-var_h))/16 ); // V*(1-(S*(1-H_Floor)))

		if      ( var_i == 0 ) { *r = v     ; *g = var_3 ; *b = var_1; } // 0   deg (r)   to 60  deg (r+g)
		else if ( var_i == 1 ) { *r = var_2 ; *g = v     ; *b = var_1; } // 60  deg (r+g) to 120 deg (g)
		else if ( var_i == 2 ) { *r = var_1 ; *g = v     ; *b = var_3; } // 120 deg (g)   to 180 deg (g+b)
		else if ( var_i == 3 ) { *r = var_1 ; *g = var_2 ; *b = v;     } // 180 deg (g+b) to 240 deg (b)
		else if ( var_i == 4 ) { *r = var_3 ; *g = var_1 ; *b = v;     } // 240 deg (b)   to 300 deg (b+r)
		else                   { *r = v     ; *g = var_1 ; *b = var_2; } // 300 deg (b+r) to 0   deg (r)
	}

}

