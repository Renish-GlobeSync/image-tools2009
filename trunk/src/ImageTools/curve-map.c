
#include <string.h>

#include "math1.h"
#include "base_enums.h"
#include "color.h"

#include "curve.h"
#include "curve-map.h"


enum
{
	CURVE_NONE   = 0,
	CURVE_COLORS = 1 << 0,
	CURVE_RED    = 1 << 1,
	CURVE_GREEN  = 1 << 2,
	CURVE_BLUE   = 1 << 3,
	CURVE_ALPHA  = 1 << 4
};

static unsigned int curve_get_apply_mask (Curve *curve_colors,
									Curve *curve_red,
									Curve *curve_green,
									Curve *curve_blue,
									Curve *curve_alpha);


double
curve_map_value (Curve *curve,
				 double    value)
{	
	if (curve->identity)
    {
		return value;
    }
	
	if (value < 0.0)
    {
		return curve->samples[0];
    }
	else if (value >= 1.0)
    {
		return curve->samples[curve->n_samples - 1];
    }
	else  /* interpolate the curve */
    {
		double f;
		int    index;
		
		/*  map value to the sample space  */
		value = value * (curve->n_samples - 1);
		
		/*  determine the indices of the closest sample points  */
		index = (int) value;
		
		/*  calculate the position between the sample points  */
		f = value - index;
		
		return (1.0 - f) * curve->samples[index] + f * curve->samples[index + 1];
    }
}

void
curve_map_pixels (Curve *curve_colors,
				  Curve *curve_red,
				  Curve *curve_green,
				  Curve *curve_blue,
				  Curve *curve_alpha,
				  unsigned char    *src,
				  unsigned char    *dest,
				  long      samples)
{	
	switch (curve_get_apply_mask (curve_colors,
								  curve_red,
								  curve_green,
								  curve_blue,
								  curve_alpha))
    {
		case CURVE_NONE:
			memcpy (dest, src, samples * 4 * sizeof (float));
			break;
			
		case CURVE_COLORS:
			while (samples--)
			{
				RGB rgb;
				rgb_set_uchar(&rgb, src[RED_PIX], src[GREEN_PIX], src[BLUE_PIX]);
				rgb.r = curve_map_value (curve_colors, rgb.r);
				rgb.g = curve_map_value (curve_colors, rgb.g);
				rgb.b = curve_map_value (curve_colors, rgb.b);
				rgb_get_uchar(&rgb, dest + RED_PIX, dest + GREEN_PIX, dest + BLUE_PIX);
				/* don't apply the colors curve to the alpha channel */
				dest[3] = src[3];
				
				src  += 4;
				dest += 4;
			}
			break;
			
		case CURVE_RED:
			while (samples--)
			{
				double r = (double) src[0] / 255.0;
				r = curve_map_value (curve_red, r);
				dest[0] = ROUND (CLAMP(r, 0.0, 1.0) * 255.0);
				dest[1] = src[1];
				dest[2] = src[2];
				dest[3] = src[3];
				
				src  += 4;
				dest += 4;
			}
			break;
			
		case CURVE_GREEN:
			while (samples--)
			{
				dest[0] = src[0];
				double g = (double) src[1] / 255.0;
				g = curve_map_value (curve_green, g);
				dest[0] = ROUND (CLAMP(g, 0.0, 1.0) * 255.0);
				dest[2] = src[2];
				dest[3] = src[3];
				
				src  += 4;
				dest += 4;
			}
			break;
			
		case CURVE_BLUE:
			while (samples--)
			{
				dest[0] = src[0];
				dest[1] = src[1];
				double b = (double) src[2] / 255.0;
				b = curve_map_value (curve_blue, b);
				dest[2] = ROUND (CLAMP(b, 0.0, 1.0) * 255.0);
				dest[3] = src[3];
				
				src  += 4;
				dest += 4;
			}
			break;
			
		case CURVE_ALPHA:
			while (samples--)
			{
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
				double a = (double) src[3] / 255.0;
				a = curve_map_value (curve_alpha, a);
				dest[3] = ROUND (CLAMP(a, 0.0, 1.0) * 255.0);
				
				src  += 4;
				dest += 4;
			}
			break;
			
		case (CURVE_RED | CURVE_GREEN | CURVE_BLUE):
			while (samples--)
			{
				RGB rgb;
				rgb_set_uchar(&rgb, src[RED_PIX], src[GREEN_PIX], src[BLUE_PIX]);
				rgb.r = curve_map_value (curve_red, rgb.r);
				rgb.g = curve_map_value (curve_green, rgb.g);
				rgb.b = curve_map_value (curve_blue, rgb.b);
				rgb_get_uchar(&rgb, dest + RED_PIX, dest + GREEN_PIX, dest + BLUE_PIX);
				dest[3] = src[3];
				
				src  += 4;
				dest += 4;
			}
			break;
			
		default:
			while (samples--)
			{
				RGB rgb;
				rgb_set_uchar(&rgb, src[RED_PIX], src[GREEN_PIX], src[BLUE_PIX]);
				rgb.r = curve_map_value (curve_colors,
										 curve_map_value (curve_red, rgb.r));
				rgb.g = curve_map_value (curve_colors,
										 curve_map_value (curve_green, rgb.g));
				rgb.b = curve_map_value (curve_colors,
										 curve_map_value (curve_blue, rgb.b));				
				rgb_get_uchar(&rgb, dest + RED_PIX, dest + GREEN_PIX, dest + BLUE_PIX);
				/* don't apply the colors curve to the alpha channel */
				dest[3] = curve_map_value (curve_alpha, src[3]);
				
				src  += 4;
				dest += 4;
			}
			break;
    }
}

static unsigned int
curve_get_apply_mask (Curve *curve_colors,
					  Curve *curve_red,
					  Curve *curve_green,
					  Curve *curve_blue,
					  Curve *curve_alpha)
{
	return ((curve_is_identity (curve_colors) ? 0 : CURVE_COLORS) |
			(curve_is_identity (curve_red)    ? 0 : CURVE_RED)    |
			(curve_is_identity (curve_green)  ? 0 : CURVE_GREEN)  |
			(curve_is_identity (curve_blue)   ? 0 : CURVE_BLUE)   |
			(curve_is_identity (curve_alpha)  ? 0 : CURVE_ALPHA));
}

