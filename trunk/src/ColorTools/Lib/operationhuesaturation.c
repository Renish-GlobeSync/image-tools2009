
#include "color.h"
#include "math1.h"
#include "base_enums.h"
#include "operationhuesaturation.h"

typedef hue_saturation_config Config;

static inline double
map_hue (const Config         *config,
         HueRange             range,
         double               value)
{
    value += (config->hue[ALL_HUES] + config->hue[range]) / 2.0;
    
    if (value < 0)
        return value + 1.0;
    else if (value > 1.0)
        return value - 1.0;
    else
        return value;
}

static inline double
map_saturation (const Config         *config,
                HueRange             range,
                double               value)
{
    double v = config->saturation[ALL_HUES] + config->saturation[range];
    
    /* This change affects the way saturation is computed. With the old
     * code (different code for value < 0), increasing the saturation
     * affected muted colors very much, and bright colors less. With the
     * new code, it affects muted colors and bright colors more or less
     * evenly. For enhancing the color in photos, the new behavior is
     * exactly what you want. It's hard for me to imagine a case in
     * which the old behavior is better.
     */
    value *= (v + 1.0);
    
    return CLAMP (value, 0.0, 1.0);
}

static inline double
map_lightness (const Config         *config,
               HueRange             range,
               double               value)
{
    double v = (config->lightness[ALL_HUES] + config->lightness[range]) / 2.0;
    
    if (v < 0)
        return value * (v + 1.0);
    else
        return value + (v * (1.0 - value));
}

bool
operation_hue_saturation (void                *in_buf,
                void                *out_buf,
                long                samples,
                const Config        *config)
{
    unsigned char                   *src    = in_buf;
    unsigned char                   *dest   = out_buf;
    double                    overlap;
    
    if (! config)
        return FALSE;
    
    overlap = config->overlap / 2.0;
    
    while (samples--)
    {
        RGB  rgb;
        HSL  hsl;
        double  h;
        int     hue_counter;
        int     hue                 = 0;
        int     secondary_hue       = 0;
        int        use_secondary_hue   = 0;
        float   primary_intensity   = 0.0;
        float   secondary_intensity = 0.0;
        
        rgb_set_uchar(&rgb, src[RED_PIX], src[GREEN_PIX], src[BLUE_PIX]);
        
        rgb_to_hsl (&rgb, &hsl);
        
        h = hsl.h * 6.0;
        
        for (hue_counter = 0; hue_counter < 7; hue_counter++)
        {
            double hue_threshold = (double) hue_counter + 0.5;
            
            if (h < ((double) hue_threshold + overlap))
            {
                hue = hue_counter;
                
                if (overlap > 0.0 && h > ((double) hue_threshold - overlap))
                {
                    use_secondary_hue = TRUE;
                    
                    secondary_hue = hue_counter + 1;
                    
                    secondary_intensity =
                    (h - (double) hue_threshold + overlap) / (2.0 * overlap);
                    
                    primary_intensity = 1.0 - secondary_intensity;
                }
                else
                {
                    use_secondary_hue = FALSE;
                }
                
                break;
            }
        }
        
        if (hue >= 6)
        {
            hue = 0;
            use_secondary_hue = FALSE;
        }
        
        if (secondary_hue >= 6)
        {
            secondary_hue = 0;
        }
        
        /*  transform into HueRange values  */
        hue++;
        secondary_hue++;
        
        if (use_secondary_hue)
        {
            double mapped_primary_hue;
            double mapped_secondary_hue;
            double diff;
            
            mapped_primary_hue   = map_hue (config, hue,           hsl.h);
            mapped_secondary_hue = map_hue (config, secondary_hue, hsl.h);
            
            /* Find nearest hue on the circle between primary and
             * secondary hue
             */
            diff = mapped_primary_hue - mapped_secondary_hue;
            if (diff < -0.5)
            {
                mapped_secondary_hue -= 1.0;
            }
            else if (diff >= 0.5)
            {
                mapped_secondary_hue += 1.0;
            }
            
            hsl.h = (mapped_primary_hue   * primary_intensity +
                     mapped_secondary_hue * secondary_intensity);
            
            hsl.s = (map_saturation (config, hue,           hsl.s) * primary_intensity +
                     map_saturation (config, secondary_hue, hsl.s) * secondary_intensity);
            
            hsl.l = (map_lightness (config, hue,           hsl.l) * primary_intensity +
                     map_lightness (config, secondary_hue, hsl.l) * secondary_intensity);
        }
        else
        {
            hsl.h = map_hue        (config, hue, hsl.h);
            hsl.s = map_saturation (config, hue, hsl.s);
            hsl.l = map_lightness  (config, hue, hsl.l);
        }
        
        hsl_to_rgb (&hsl, &rgb);
        
        rgb_get_uchar(&rgb, dest + RED_PIX, dest + GREEN_PIX, dest + BLUE_PIX);
        dest[ALPHA_PIX] = src[ALPHA_PIX];
        
        src  += 4;
        dest += 4;
    }
    
    return TRUE;
}

void
operation_hue_saturation_map (const Config        *config,
                    const RGB           *color,
                    HueRange            range,
                    RGB                 *result)
{
    HSL hsl;
    
    rgb_to_hsl (color, &hsl);
    
    hsl.h = map_hue        (config, range, hsl.h);
    hsl.s = map_saturation (config, range, hsl.s);
    hsl.l = map_lightness  (config, range, hsl.l);
    
    hsl_to_rgb (&hsl, result);
}
