
#include "color.h"

#include "operationcolorize.h"

typedef colorize_config Config;

bool
operation_colorize (void                *in_buf,
		  void                *out_buf,
		  long                samples,
		  const Config        *config)
{
	unsigned char                   *src    = in_buf;
	unsigned char                   *dest   = out_buf;
	HSL                   hsl;
	
	if (! config)
		return FALSE;
	
	hsl.h = config->hue;
	hsl.s = config->saturation;
	
	while (samples--)
    {
		RGB rgb;
		double  lum;
		
		rgb_set_uchar(&rgb, src[RED_PIX], src[GREEN_PIX], src[BLUE_PIX]);
		lum = RGB_LUMINANCE (rgb.r, rgb.g, rgb.b);

		if (config->lightness > 0)
        {
			lum = lum * (1.0 - config->lightness);
			
			lum += 1.0 - (1.0 - config->lightness);
        }
		else if (config->lightness < 0)
        {
			lum = lum * (config->lightness + 1.0);
        }
		
		hsl.l = lum;
		
		hsl_to_rgb (&hsl, &rgb);
		
		rgb_get_uchar(&rgb, dest + RED_PIX, dest + GREEN_PIX, dest + BLUE_PIX);
		dest[ALPHA_PIX] = src[ALPHA_PIX];
		
		src  += 4;
		dest += 4;
    }
	
	return TRUE;
}
