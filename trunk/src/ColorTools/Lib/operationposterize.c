
#include "color.h"
#include "math1.h"

#include "operationposterize.h"

typedef posterize_config Config;

bool
operation_posterize (void                *in_buf,
					 void                *out_buf,
					 long                samples,
					 const Config        *config)
{
	unsigned char             *src    = in_buf;
	unsigned char             *dest   = out_buf;
	double                    levels;
	
	if (! config)
		return FALSE;
	
	levels = config->levels - 1.0;
	
	while (samples--)
    {
		RGB rgb;
		rgb_set_uchar(&rgb, src[RED_PIX], src[GREEN_PIX], src[BLUE_PIX]);
		rgb.r = RINT (rgb.r * levels) / levels;
		rgb.g = RINT (rgb.g * levels) / levels;
		rgb.b = RINT (rgb.b * levels) / levels;
		rgb_get_uchar(&rgb, dest + RED_PIX, dest + GREEN_PIX, dest + BLUE_PIX);
		dest[ALPHA_PIX] = src[ALPHA_PIX];
		
		src  += 4;
		dest += 4;
    }
	
	return TRUE;
}
