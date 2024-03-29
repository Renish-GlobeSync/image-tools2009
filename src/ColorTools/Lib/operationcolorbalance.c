
#include "color.h"
#include "math1.h"
#include "base_enums.h"
#include "operationcolorbalance.h"

static inline double
color_balance_map (double  value,
                   double shadows,
                   double midtones,
                   double highlights)
{
    double low = 1.075 - 1.0 / (value / 16.0 + 1.0);
    double mid = 0.667 * (1.0 - SQR (value - 0.5));
    double shadows_add;
    double shadows_sub;
    double midtones_add;
    double midtones_sub;
    double highlights_add;
    double highlights_sub;

    shadows_add    = low + 1.0;
    shadows_sub    = 1.0 - low;

    midtones_add   = mid;
    midtones_sub   = mid;

    highlights_add = 1.0 - low;
    highlights_sub = low + 1.0;

    value += shadows * (shadows > 0 ? shadows_add : shadows_sub);
    value = CLAMP (value, 0.0, 1.0);

    value += midtones * (midtones > 0 ? midtones_add : midtones_sub);
    value = CLAMP (value, 0.0, 1.0);

    value += highlights * (highlights > 0 ? highlights_add : highlights_sub);
    value = CLAMP (value, 0.0, 1.0);

    return value;
}

typedef color_balance_config Config;

bool
operation_color_balance(void * in_buf,
                        void * out_buf,
                        long samples, 
                        const Config * config)
{
    unsigned char * src = in_buf;
    unsigned char * dest = out_buf;

    if (! config)
        return FALSE;

    while (samples--)
    {
        RGB rgb;
        RGB rgb_n;

        rgb_set_uchar(&rgb, src[RED_PIX], src[GREEN_PIX], src[BLUE_PIX]);

        rgb_n.r = color_balance_map (rgb.r,
            config->cyan_red[SHADOWS],
            config->cyan_red[MIDTONES],
            config->cyan_red[HIGHLIGHTS]);

        rgb_n.g = color_balance_map (rgb.g,
            config->magenta_green[SHADOWS],
            config->magenta_green[MIDTONES],
            config->magenta_green[HIGHLIGHTS]);

        rgb_n.b = color_balance_map (rgb.b,
            config->yellow_blue[SHADOWS],
            config->yellow_blue[MIDTONES],
            config->yellow_blue[HIGHLIGHTS]);

        if (config->preserve_luminosity)
        {
            HSL hsl;
            HSL hsl_n;

            rgb_to_hsl (&rgb_n, &hsl_n);

            rgb_to_hsl (&rgb, &hsl);

            hsl_n.l = hsl.l;

            hsl_to_rgb (&hsl_n, &rgb_n);

        }

        rgb_get_uchar(&rgb_n, dest + RED_PIX, dest + GREEN_PIX, dest + BLUE_PIX);
        dest[ALPHA_PIX] = src[ALPHA_PIX];

        src  += 4;
        dest += 4;
    }

    return TRUE;
}

// optimized 

static inline unsigned char
color_balance_map_uchar (unsigned char value,
                         double shadows,
                         double midtones,
                         double highlights)
{
    double v = color_balance_map((double) value / 255.0, shadows, midtones, highlights);
    return ROUND(v * 255.0);
}

typedef struct
{
    unsigned char cyan_red_map[256];
    unsigned char magenta_green_map[256];
    unsigned char yellow_blue_map[256];
} color_balance_temp;

bool
operation_color_balance_optimize(void * in_buf,
                                 void * out_buf,
                                 long samples, 
                                 const Config * config)
{
    color_balance_temp temp;

    unsigned char * src = in_buf;
    unsigned char * dest = out_buf;

    unsigned short c = 0;

    if (! config)
        return FALSE;

    for (; c < 256; c++) {
        temp.cyan_red_map[c] = color_balance_map_uchar ((unsigned char)c, 
            config->cyan_red[SHADOWS],
            config->cyan_red[MIDTONES],
            config->cyan_red[HIGHLIGHTS]);

        temp.magenta_green_map[c] = color_balance_map_uchar ((unsigned char)c, 
            config->magenta_green[SHADOWS],
            config->magenta_green[MIDTONES],
            config->magenta_green[HIGHLIGHTS]);

        temp.yellow_blue_map[c] = color_balance_map_uchar ((unsigned char)c, 
            config->yellow_blue[SHADOWS],
            config->yellow_blue[MIDTONES],
            config->yellow_blue[HIGHLIGHTS]);
    }

    while (samples--) {
        dest[RED_PIX] = temp.cyan_red_map[src[RED_PIX]];
        dest[GREEN_PIX] = temp.magenta_green_map[src[GREEN_PIX]];
        dest[BLUE_PIX] = temp.yellow_blue_map[src[BLUE_PIX]];
        dest[ALPHA_PIX] = src[ALPHA_PIX];

        src  += 4;
        dest += 4;
    }

    return TRUE;
}
