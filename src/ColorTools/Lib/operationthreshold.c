
#include "color.h"
#include "math1.h"

#include "operationthreshold.h"

typedef threshold_config Config;

bool
operation_threshold (void                *in_buf,
           void                *out_buf,
           long                samples,
           const Config        *config)
{
    unsigned char         *src    = in_buf;
    unsigned char         *dest   = out_buf;
    
    unsigned char low = config->low * 255;
    unsigned char high = config->high * 255;
    
    if (! config)
        return FALSE;
    
    while (samples--)
    {
        unsigned char value;
        
        value = MAX (src[RED_PIX], src[GREEN_PIX]);
        value = MAX (value, src[BLUE_PIX]);
        
        value = (value >= low && value <= high) ? 255 : 0;
        
        dest[RED_PIX]   = value;
        dest[GREEN_PIX] = value;
        dest[BLUE_PIX]  = value;
        dest[ALPHA_PIX] = src[ALPHA_PIX];
        
        src  += 4;
        dest += 4;
    }
    
    return TRUE;
}
