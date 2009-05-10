
#include "colortypes.h"

#include "colorspace.h"
#include "math1.h"

#include "rgb.h"

//#include "hsv.h"


#define HSV_UNDEFINED -1.0
#define HSL_UNDEFINED -1.0

/*********************************
 *   color conversion routines   *
 *********************************/


/*  RGB functions  */


/**
 * rgb_to_hsv:
 * @rgb: A color value in the RGB colorspace
 * @hsv: The value converted to the HSV colorspace
 *
 * Does a conversion from RGB to HSV (Hue, Saturation,
 * Value) colorspace.
 **/
void
rgb_to_hsv (const RGB *rgb,
            HSV       *hsv)
{
    double max, min, delta;
    
    max = rgb_max (rgb);
    min = rgb_min (rgb);
    
    hsv->v = max;
    delta = max - min;
    
    if (delta > 0.0001)
    {
        hsv->s = delta / max;
        
        if (rgb->r == max)
        {
            hsv->h = (rgb->g - rgb->b) / delta;
            if (hsv->h < 0.0)
                hsv->h += 6.0;
        }
        else if (rgb->g == max)
        {
            hsv->h = 2.0 + (rgb->b - rgb->r) / delta;
        }
        else if (rgb->b == max)
        {
            hsv->h = 4.0 + (rgb->r - rgb->g) / delta;
        }
        
        hsv->h /= 6.0;
    }
    else
    {
        hsv->s = 0.0;
        hsv->h = 0.0;
    }
    
    hsv->a = rgb->a;
}

/**
 * hsv_to_rgb:
 * @hsv: A color value in the HSV colorspace
 * @rgb: The returned RGB value.
 *
 * Converts a color value from HSV to RGB colorspace
 **/
void
hsv_to_rgb (const HSV *hsv,
            RGB       *rgb)
{
    int    i;
    double f, w, q, t;
    
    double hue;
    
    if (hsv->s == 0.0)
    {
        rgb->r = hsv->v;
        rgb->g = hsv->v;
        rgb->b = hsv->v;
    }
    else
    {
        hue = hsv->h;
        
        if (hue == 1.0)
            hue = 0.0;
        
        hue *= 6.0;
        
        i = (int) hue;
        f = hue - i;
        w = hsv->v * (1.0 - hsv->s);
        q = hsv->v * (1.0 - (hsv->s * f));
        t = hsv->v * (1.0 - (hsv->s * (1.0 - f)));
        
        switch (i)
        {
            case 0:
                rgb->r = hsv->v;
                rgb->g = t;
                rgb->b = w;
                break;
            case 1:
                rgb->r = q;
                rgb->g = hsv->v;
                rgb->b = w;
                break;
            case 2:
                rgb->r = w;
                rgb->g = hsv->v;
                rgb->b = t;
                break;
            case 3:
                rgb->r = w;
                rgb->g = q;
                rgb->b = hsv->v;
                break;
            case 4:
                rgb->r = t;
                rgb->g = w;
                rgb->b = hsv->v;
                break;
            case 5:
                rgb->r = hsv->v;
                rgb->g = w;
                rgb->b = q;
                break;
        }
    }
    
    rgb->a = hsv->a;
}


/**
 * rgb_to_hsl:
 * @rgb: A color value in the RGB colorspace
 * @hsl: The value converted to HSL
 *
 * Convert an RGB color value to a HSL (Hue, Saturation, Lightness)
 * color value.
 **/
void
rgb_to_hsl (const RGB *rgb,
            HSL       *hsl)
{
    double max, min, delta;
    
    max = rgb_max (rgb);
    min = rgb_min (rgb);
    
    hsl->l = (max + min) / 2.0;
    
    if (max == min)
    {
        hsl->s = 0.0;
        hsl->h = HSL_UNDEFINED;
    }
    else
    {
        if (hsl->l <= 0.5)
            hsl->s = (max - min) / (max + min);
        else
            hsl->s = (max - min) / (2.0 - max - min);
        
        delta = max - min;
        
        if (delta == 0.0)
            delta = 1.0;
        
        if (rgb->r == max)
        {
            hsl->h = (rgb->g - rgb->b) / delta;
        }
        else if (rgb->g == max)
        {
            hsl->h = 2.0 + (rgb->b - rgb->r) / delta;
        }
        else if (rgb->b == max)
        {
            hsl->h = 4.0 + (rgb->r - rgb->g) / delta;
        }
        
        hsl->h /= 6.0;
        
        if (hsl->h < 0.0)
            hsl->h += 1.0;
    }
    
    hsl->a = rgb->a;
}

static inline double
hsl_value (double n1,
           double n2,
           double hue)
{
    double val;
    
    if (hue > 6.0)
        hue -= 6.0;
    else if (hue < 0.0)
        hue += 6.0;
    
    if (hue < 1.0)
        val = n1 + (n2 - n1) * hue;
    else if (hue < 3.0)
        val = n2;
    else if (hue < 4.0)
        val = n1 + (n2 - n1) * (4.0 - hue);
    else
        val = n1;
    
    return val;
}


/**
 * hsl_to_rgb:
 * @hsl: A color value in the HSL colorspace
 * @rgb: The value converted to a value in the RGB colorspace
 *
 * Convert a HSL color value to an RGB color value.
 **/
void
hsl_to_rgb (const HSL *hsl,
            RGB       *rgb)
{
    if (hsl->s == 0)
    {
        /*  achromatic case  */
        rgb->r = hsl->l;
        rgb->g = hsl->l;
        rgb->b = hsl->l;
    }
    else
    {
        double m1, m2;
        
        if (hsl->l <= 0.5)
            m2 = hsl->l * (1.0 + hsl->s);
        else
            m2 = hsl->l + hsl->s - hsl->l * hsl->s;
        
        m1 = 2.0 * hsl->l - m2;
        
        rgb->r = hsl_value (m1, m2, hsl->h * 6.0 + 2.0);
        rgb->g = hsl_value (m1, m2, hsl->h * 6.0);
        rgb->b = hsl_value (m1, m2, hsl->h * 6.0 - 2.0);
    }
    
    rgb->a = hsl->a;
}


/**
 * rgb_to_cmyk:
 * @rgb: A value in the RGB colorspace
 * @pullout: A scaling value (0-1) indicating how much black should be
 *           pulled out
 * @cmyk: The input value naively converted to the CMYK colorspace
 *
 * Does a naive conversion from RGB to CMYK colorspace. A simple
 * formula that doesn't take any color-profiles into account is used.
 * The amount of black pullout how can be controlled via the @pullout
 * parameter. A @pullout value of 0 makes this a conversion to CMY.
 * A value of 1 causes the maximum amount of black to be pulled out.
 **/
void
rgb_to_cmyk (const RGB  *rgb,
             double         pullout,
             CMYK       *cmyk)
{
    double c, m, y, k;
    
    c = 1.0 - rgb->r;
    m = 1.0 - rgb->g;
    y = 1.0 - rgb->b;
    
    k = 1.0;
    if (c < k)  k = c;
    if (m < k)  k = m;
    if (y < k)  k = y;
    
    k *= pullout;
    
    if (k < 1.0)
    {
        cmyk->c = (c - k) / (1.0 - k);
        cmyk->m = (m - k) / (1.0 - k);
        cmyk->y = (y - k) / (1.0 - k);
    }
    else
    {
        cmyk->c = 0.0;
        cmyk->m = 0.0;
        cmyk->y = 0.0;
    }
    
    cmyk->k = k;
    cmyk->a = rgb->a;
}

/**
 * cmyk_to_rgb:
 * @cmyk: A color value in the CMYK colorspace
 * @rgb: The value converted to the RGB colorspace
 *
 * Does a simple transformation from the CMYK colorspace to the RGB
 * colorspace, without taking color profiles into account.
 **/
void
cmyk_to_rgb (const CMYK *cmyk,
             RGB        *rgb)
{
    double c, m, y, k;
    
    k = cmyk->k;
    
    if (k < 1.0)
    {
        c = cmyk->c * (1.0 - k) + k;
        m = cmyk->m * (1.0 - k) + k;
        y = cmyk->y * (1.0 - k) + k;
    }
    else
    {
        c = 1.0;
        m = 1.0;
        y = 1.0;
    }
    
    rgb->r = 1.0 - c;
    rgb->g = 1.0 - m;
    rgb->b = 1.0 - y;
    rgb->a = cmyk->a;
}


#define RETURN_RGB(x, y, z) { rgb->r = x; rgb->g = y; rgb->b = z; return; }

/****************************************************************************
 * Theoretically, hue 0 (pure red) is identical to hue 6 in these transforms.
 * Pure red always maps to 6 in this implementation. Therefore UNDEFINED can
 * be defined as 0 in situations where only unsigned numbers are desired.
 ****************************************************************************/

/**
 * rgb_to_hwb:
 * @rgb: A color value in the RGB colorspace
 * @hue: The hue value of the above color, in the range 0 to 6
 * @whiteness: The whiteness value of the above color, in the range 0 to 1
 * @blackness: The blackness value of the above color, in the range 0 to 1
 *
 * Theoretically, hue 0 (pure red) is identical to hue 6 in these transforms.
 * Pure red always maps to 6 in this implementation. Therefore UNDEFINED can
 * be defined as 0 in situations where only unsigned numbers are desired.
 *
 * RGB are each on [0, 1]. Whiteness and Blackness are returned in the
 * range [0, 1] and H is returned in the range [0, 6]. If W == 1 - B, H is
 * undefined.
 **/
void
rgb_to_hwb (const RGB *rgb,
            double       *hue,
            double       *whiteness,
            double       *blackness)
{
    /* RGB are each on [0, 1]. W and B are returned on [0, 1] and H is        */
    /* returned on [0, 6]. Exception: H is returned UNDEFINED if W ==  1 - B. */
    /* ====================================================================== */
    
    double R = rgb->r, G = rgb->g, B = rgb->b, w, v, b, f;
    int i;
    
    w = rgb_min (rgb);
    v = rgb_max (rgb);
    b = 1.0 - v;
    
    if (v == w)
    {
        *hue = HSV_UNDEFINED;
        *whiteness = w;
        *blackness = b;
    }
    else
    {
        f = (R == w) ? G - B : ((G == w) ? B - R : R - G);
        i = (R == w) ? 3.0 : ((G == w) ? 5.0 : 1.0);
        
        *hue = (360.0 / 6.0) * (i - f / (v - w));
        *whiteness = w;
        *blackness = b;
    }
}

/**
 * hwb_to_rgb:
 * @hue: A hue value, in the range 0 to 6
 * @whiteness: A whiteness value, in the range 0 to 1
 * @blackness: A blackness value, in the range 0 to 1
 * @rgb: The above color converted to the RGB colorspace
 *
 * H is defined in the range [0, 6] or UNDEFINED, B and W are both in the
 * range [0, 1]. The returned RGB values are all in the range [0, 1].
 **/
void
hwb_to_rgb (double  hue,
            double  whiteness,
            double  blackness,
            RGB *rgb)
{
    /* H is given on [0, 6] or UNDEFINED. whiteness and
     * blackness are given on [0, 1].
     * RGB are each returned on [0, 1].
     */
    
    double h = hue, w = whiteness, b = blackness, v, n, f;
    int    i;
    
    h = 6.0 * h/ 360.0;
    
    v = 1.0 - b;
    if (h == HSV_UNDEFINED)
    {
        rgb->r = v;
        rgb->g = v;
        rgb->b = v;
    }
    else
    {
        i = floor (h);
        f = h - i;
        
        if (i & 1)
            f = 1.0 - f;  /* if i is odd */
        
        n = w + f * (v - w);     /* linear interpolation between w and v */
        
        switch (i)
        {
            case 6:
            case 0: RETURN_RGB (v, n, w);
                break;
            case 1: RETURN_RGB (n, v, w);
                break;
            case 2: RETURN_RGB (w, v, n);
                break;
            case 3: RETURN_RGB (w, n, v);
                break;
            case 4: RETURN_RGB (n, w, v);
                break;
            case 5: RETURN_RGB (v, w, n);
                break;
        }
    }
    
}


/*  int functions  */

/**
 * rgb_to_hsv_int:
 * @red: The red channel value, returns the Hue channel
 * @green: The green channel value, returns the Saturation channel
 * @blue: The blue channel value, returns the Value channel
 *
 * The arguments are pointers to int representing channel values in
 * the RGB colorspace, and the values pointed to are all in the range
 * [0, 255].
 *
 * The function changes the arguments to point to the HSV value
 * corresponding, with the returned values in the following
 * ranges: H [0, 359], S [0, 255], V [0, 255].
 **/
void
rgb_to_hsv_int (int *red,
                int *green,
                int *blue)
{
    double  r, g, b;
    double  h, s, v;
    int     min;
    double  delta;
    
    r = *red;
    g = *green;
    b = *blue;
    
    if (r > g)
    {
        v = MAX (r, b);
        min = MIN (g, b);
    }
    else
    {
        v = MAX (g, b);
        min = MIN (r, b);
    }
    
    delta = v - min;
    
    if (v == 0.0)
        s = 0.0;
    else
        s = delta / v;
    
    if (s == 0.0)
    {
        h = 0.0;
    }
    else
    {
        if (r == v)
            h = 60.0 * (g - b) / delta;
        else if (g == v)
            h = 120 + 60.0 * (b - r) / delta;
        else
            h = 240 + 60.0 * (r - g) / delta;
        
        if (h < 0.0)
            h += 360.0;
        
        if (h > 360.0)
            h -= 360.0;
    }
    
    *red   = ROUND (h);
    *green = ROUND (s * 255.0);
    *blue  = ROUND (v);
    
    /* avoid the ambiguity of returning different values for the same color */
    if (*red == 360)
        *red = 0;
}

/**
 * hsv_to_rgb_int:
 * @hue: The hue channel, returns the red channel
 * @saturation: The saturation channel, returns the green channel
 * @value: The value channel, returns the blue channel
 *
 * The arguments are pointers to int, with the values pointed to in the
 * following ranges:  H [0, 360], S [0, 255], V [0, 255].
 *
 * The function changes the arguments to point to the RGB value
 * corresponding, with the returned values all in the range [0, 255].
 **/
void
hsv_to_rgb_int (int *hue,
                int *saturation,
                int *value)
{
    double h, s, v, h_temp;
    double f, p, q, t;
    int i;
    
    if (*saturation == 0)
    {
        *hue        = *value;
        *saturation = *value;
        *value      = *value;
    }
    else
    {
        h = *hue;
        s = *saturation / 255.0;
        v = *value      / 255.0;
        
        if (h == 360)
            h_temp = 0;
        else
            h_temp = h;
        
        h_temp = h_temp / 60.0;
        i = floor (h_temp);
        f = h_temp - i;
        p = v * (1.0 - s);
        q = v * (1.0 - (s * f));
        t = v * (1.0 - (s * (1.0 - f)));
        
        switch (i)
        {
            case 0:
                *hue        = ROUND (v * 255.0);
                *saturation = ROUND (t * 255.0);
                *value      = ROUND (p * 255.0);
                break;
                
            case 1:
                *hue        = ROUND (q * 255.0);
                *saturation = ROUND (v * 255.0);
                *value      = ROUND (p * 255.0);
                break;
                
            case 2:
                *hue        = ROUND (p * 255.0);
                *saturation = ROUND (v * 255.0);
                *value      = ROUND (t * 255.0);
                break;
                
            case 3:
                *hue        = ROUND (p * 255.0);
                *saturation = ROUND (q * 255.0);
                *value      = ROUND (v * 255.0);
                break;
                
            case 4:
                *hue        = ROUND (t * 255.0);
                *saturation = ROUND (p * 255.0);
                *value      = ROUND (v * 255.0);
                break;
                
            case 5:
                *hue        = ROUND (v * 255.0);
                *saturation = ROUND (p * 255.0);
                *value      = ROUND (q * 255.0);
                break;
        }
    }
}

/**
 * rgb_to_hsl_int:
 * @red: Red channel, returns Hue channel
 * @green: Green channel, returns Lightness channel
 * @blue: Blue channel, returns Saturation channel
 *
 * The arguments are pointers to int representing channel values in the
 * RGB colorspace, and the values pointed to are all in the range [0, 255].
 *
 * The function changes the arguments to point to the corresponding HLS
 * value with the values pointed to in the following ranges:  H [0, 360],
 * L [0, 255], S [0, 255].
 **/
void
rgb_to_hsl_int (int *red,
                int *green,
                int *blue)
{
    int    r, g, b;
    double h, s, l;
    int    min, max;
    int    delta;
    
    r = *red;
    g = *green;
    b = *blue;
    
    if (r > g)
    {
        max = MAX (r, b);
        min = MIN (g, b);
    }
    else
    {
        max = MAX (g, b);
        min = MIN (r, b);
    }
    
    l = (max + min) / 2.0;
    
    if (max == min)
    {
        s = 0.0;
        h = 0.0;
    }
    else
    {
        delta = (max - min);
        
        if (l < 128)
            s = 255 * (double) delta / (double) (max + min);
        else
            s = 255 * (double) delta / (double) (511 - max - min);
        
        if (r == max)
            h = (g - b) / (double) delta;
        else if (g == max)
            h = 2 + (b - r) / (double) delta;
        else
            h = 4 + (r - g) / (double) delta;
        
        h = h * 42.5;
        
        if (h < 0)
            h += 255;
        else if (h > 255)
            h -= 255;
    }
    
    *red   = ROUND (h);
    *green = ROUND (s);
    *blue  = ROUND (l);
}

/**
 * rgb_to_l_int:
 * @red: Red channel
 * @green: Green channel
 * @blue: Blue channel
 *
 * Calculates the lightness value of an RGB triplet with the formula
 * L = (max(R, G, B) + min (R, G, B)) / 2
 *
 * Return value: Luminance vaue corresponding to the input RGB value
 **/
int
rgb_to_l_int (int red,
              int green,
              int blue)
{
    int min, max;
    
    if (red > green)
    {
        max = MAX (red,   blue);
        min = MIN (green, blue);
    }
    else
    {
        max = MAX (green, blue);
        min = MIN (red,   blue);
    }
    
    return ROUND ((max + min) / 2.0);
}

static inline int
hsl_value_int (double n1,
               double n2,
               double hue)
{
    double value;
    
    if (hue > 255)
        hue -= 255;
    else if (hue < 0)
        hue += 255;
    
    if (hue < 42.5)
        value = n1 + (n2 - n1) * (hue / 42.5);
    else if (hue < 127.5)
        value = n2;
    else if (hue < 170)
        value = n1 + (n2 - n1) * ((170 - hue) / 42.5);
    else
        value = n1;
    
    return ROUND (value * 255.0);
}

/**
 * hsl_to_rgb_int:
 * @hue: Hue channel, returns Red channel
 * @saturation: Saturation channel, returns Green channel
 * @lightness: Lightness channel, returns Blue channel
 *
 * The arguments are pointers to int, with the values pointed to in the
 * following ranges:  H [0, 360], L [0, 255], S [0, 255].
 *
 * The function changes the arguments to point to the RGB value
 * corresponding, with the returned values all in the range [0, 255].
 **/
void
hsl_to_rgb_int (int *hue,
                int *saturation,
                int *lightness)
{
    double h, s, l;
    
    h = *hue;
    s = *saturation;
    l = *lightness;
    
    if (s == 0)
    {
        /*  achromatic case  */
        *hue        = l;
        *lightness  = l;
        *saturation = l;
    }
    else
    {
        double m1, m2;
        
        if (l < 128)
            m2 = (l * (255 + s)) / 65025.0;
        else
            m2 = (l + s - (l * s) / 255.0) / 255.0;
        
        m1 = (l / 127.5) - m2;
        
        /*  chromatic case  */
        *hue        = hsl_value_int (m1, m2, h + 85);
        *saturation = hsl_value_int (m1, m2, h);
        *lightness  = hsl_value_int (m1, m2, h - 85);
    }
}

/**
 * rgb_to_cmyk_int:
 * @red:     the red channel; returns the cyan value (0-255)
 * @green:   the green channel; returns the magenta value (0-255)
 * @blue:    the blue channel; returns the yellow value (0-255)
 * @pullout: the percentage of black to pull out (0-100); returns
 *           the black value (0-255)
 *
 * Does a naive conversion from RGB to CMYK colorspace. A simple
 * formula that doesn't take any color-profiles into account is used.
 * The amount of black pullout how can be controlled via the @pullout
 * parameter. A @pullout value of 0 makes this a conversion to CMY.
 * A value of 100 causes the maximum amount of black to be pulled out.
 **/
void
rgb_to_cmyk_int (int *red,
                 int *green,
                 int *blue,
                 int *pullout)
{
    int c, m, y;
    
    c = 255 - *red;
    m = 255 - *green;
    y = 255 - *blue;
    
    if (*pullout == 0)
    {
        *red   = c;
        *green = m;
        *blue  = y;
    }
    else
    {
        int k = 255;
        
        if (c < k)  k = c;
        if (m < k)  k = m;
        if (y < k)  k = y;
        
        k = (k * CLAMP (*pullout, 0, 100)) / 100;
        
        *red   = ((c - k) << 8) / (256 - k);
        *green = ((m - k) << 8) / (256 - k);
        *blue  = ((y - k) << 8) / (256 - k);
        *pullout = k;
    }
}

/**
 * cmyk_to_rgb_int:
 * @cyan:    the cyan channel; returns the red value (0-255)
 * @magenta: the magenta channel; returns the green value (0-255)
 * @yellow:  the yellow channel; returns the blue value (0-255)
 * @black:   the black channel (0-255); doesn't change
 *
 * Does a naive conversion from CMYK to RGB colorspace. A simple
 * formula that doesn't take any color-profiles into account is used.
 **/
void
cmyk_to_rgb_int (int *cyan,
                 int *magenta,
                 int *yellow,
                 int *black)
{
    int c, m, y, k;
    
    c = *cyan;
    m = *magenta;
    y = *yellow;
    k = *black;
    
    if (k)
    {
        c = ((c * (256 - k)) >> 8) + k;
        m = ((m * (256 - k)) >> 8) + k;
        y = ((y * (256 - k)) >> 8) + k;
    }
    
    *cyan    = 255 - c;
    *magenta = 255 - m;
    *yellow  = 255 - y;
}

/**
 * rgb_to_hsv4:
 * @rgb:        RGB triplet, rgb[0] is red channel, rgb[1] is green,
 *              rgb[2] is blue (0..255)
 * @hue:        Pointer to hue channel (0..1)
 * @saturation: Pointer to saturation channel (0..1)
 * @value:      Pointer to value channel (0..1)
 **/
void
rgb_to_hsv4 (const unsigned char *rgb,
             double      *hue,
             double      *saturation,
             double      *value)
{
    double red, green, blue;
    double h, s, v;
    double min, max;
    double delta;
    
    red   = rgb[0] / 255.0;
    green = rgb[1] / 255.0;
    blue  = rgb[2] / 255.0;
    
    h = 0.0; /* Shut up -Wall */
    
    if (red > green)
    {
        max = MAX (red,   blue);
        min = MIN (green, blue);
    }
    else
    {
        max = MAX (green, blue);
        min = MIN (red,   blue);
    }
    
    v = max;
    
    if (max != 0.0)
        s = (max - min) / max;
    else
        s = 0.0;
    
    if (s == 0.0)
        h = 0.0;
    else
    {
        delta = max - min;
        
        if (delta == 0.0)
            delta = 1.0;
        
        if (red == max)
            h = (green - blue) / delta;
        else if (green == max)
            h = 2 + (blue - red) / delta;
        else if (blue == max)
            h = 4 + (red - green) / delta;
        
        h /= 6.0;
        
        if (h < 0.0)
            h += 1.0;
        else if (h > 1.0)
            h -= 1.0;
    }
    
    *hue        = h;
    *saturation = s;
    *value      = v;
}

/**
 * hsv_to_rgb4:
 * @rgb:        RGB triplet, rgb[0] is red channel, rgb[1] is green,
 *              rgb[2] is blue (0..255)
 * @hue:        Hue channel (0..1)
 * @saturation: Saturation channel (0..1)
 * @value:      Value channel (0..1)
 **/
void
hsv_to_rgb4 (unsigned char  *rgb,
             double  hue,
             double  saturation,
             double  value)
{
    double h, s, v;
    double f, p, q, t;
    
    if (saturation == 0.0)
    {
        hue        = value;
        saturation = value;
        value      = value;
    }
    else
    {
        h = hue * 6.0;
        s = saturation;
        v = value;
        
        if (h == 6.0)
            h = 0.0;
        
        f = h - (int) h;
        p = v * (1.0 - s);
        q = v * (1.0 - s * f);
        t = v * (1.0 - s * (1.0 - f));
        
        switch ((int) h)
        {
            case 0:
                hue        = v;
                saturation = t;
                value      = p;
                break;
                
            case 1:
                hue        = q;
                saturation = v;
                value      = p;
                break;
                
            case 2:
                hue        = p;
                saturation = v;
                value      = t;
                break;
                
            case 3:
                hue        = p;
                saturation = q;
                value      = v;
                break;
                
            case 4:
                hue        = t;
                saturation = p;
                value      = v;
                break;
                
            case 5:
                hue        = v;
                saturation = p;
                value      = q;
                break;
        }
    }
    
    rgb[0] = ROUND (hue        * 255.0);
    rgb[1] = ROUND (saturation * 255.0);
    rgb[2] = ROUND (value      * 255.0);
}
