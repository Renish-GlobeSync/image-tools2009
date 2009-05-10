
#include "math1.h"

#include "colortypes.h"

#include "rgb.h"

/*  RGB functions  */

/**
 * rgb_set:
 * @rgb: a #RGB struct
 * @red:
 * @green:
 * @blue:
 *
 * Sets the red, green and blue components of @rgb and leaves the
 * alpha component unchanged. The color values should be between 0.0
 * and 1.0 but there is no check to enforce this and the values are
 * set exactly as they are passed in.
 **/
void
rgb_set (RGB *rgb,
         double  r,
         double  g,
         double  b)
{
    rgb->r = r;
    rgb->g = g;
    rgb->b = b;
}

/**
 * rgb_set_alpha:
 * @rgb: a #RGB struct
 * @alpha:
 *
 * Sets the alpha component of @rgb and leaves the RGB components unchanged.
 **/
void
rgb_set_alpha (RGB *rgb,
               double  a)
{
    rgb->a = a;
}

/**
 * rgb_set_uchar:
 * @rgb: a #RGB struct
 * @red:
 * @green:
 * @blue:
 *
 * Sets the red, green and blue components of @rgb from 8bit values
 * (0 to 255) and leaves the alpha component unchanged.
 **/
void
rgb_set_uchar (RGB *rgb,
               unsigned char   r,
               unsigned char   g,
               unsigned char   b)
{
    rgb->r = (double) r / 255.0;
    rgb->g = (double) g / 255.0;
    rgb->b = (double) b / 255.0;
}

void
rgb_get_uchar (const RGB *rgb,
               unsigned char        *r,
               unsigned char        *g,
               unsigned char        *b)
{
    if (r) *r = ROUND (CLAMP (rgb->r, 0.0, 1.0) * 255.0);
    if (g) *g = ROUND (CLAMP (rgb->g, 0.0, 1.0) * 255.0);
    if (b) *b = ROUND (CLAMP (rgb->b, 0.0, 1.0) * 255.0);
}

void
rgb_add (RGB       *rgb1,
         const RGB *rgb2)
{
    rgb1->r += rgb2->r;
    rgb1->g += rgb2->g;
    rgb1->b += rgb2->b;
}

void
rgb_subtract (RGB       *rgb1,
              const RGB *rgb2)
{
    rgb1->r -= rgb2->r;
    rgb1->g -= rgb2->g;
    rgb1->b -= rgb2->b;
}

void
rgb_multiply (RGB *rgb,
              double  factor)
{
    rgb->r *= factor;
    rgb->g *= factor;
    rgb->b *= factor;
}

double
rgb_distance (const RGB *rgb1,
              const RGB *rgb2)
{
    return (fabs (rgb1->r - rgb2->r) +
            fabs (rgb1->g - rgb2->g) +
            fabs (rgb1->b - rgb2->b));
}

double
rgb_max (const RGB *rgb)
{
    if (rgb->r > rgb->g)
        return (rgb->r > rgb->b) ? rgb->r : rgb->b;
    else
        return (rgb->g > rgb->b) ? rgb->g : rgb->b;
}

double
rgb_min (const RGB *rgb)
{
    if (rgb->r < rgb->g)
        return (rgb->r < rgb->b) ? rgb->r : rgb->b;
    else
        return (rgb->g < rgb->b) ? rgb->g : rgb->b;
}

void
rgb_clamp (RGB *rgb)
{
    rgb->r = CLAMP (rgb->r, 0.0, 1.0);
    rgb->g = CLAMP (rgb->g, 0.0, 1.0);
    rgb->b = CLAMP (rgb->b, 0.0, 1.0);
    rgb->a = CLAMP (rgb->a, 0.0, 1.0);
}

void
rgb_gamma (RGB *rgb,
           double  gamma)
{
    double ig;
    
    if (gamma != 0.0)
        ig = 1.0 / gamma;
    else
        ig = 0.0;
    
    rgb->r = pow (rgb->r, ig);
    rgb->g = pow (rgb->g, ig);
    rgb->b = pow (rgb->b, ig);
}

/**
 * rgb_luminance:
 * @rgb:
 *
 * Return value: the luminous intensity of the range from 0.0 to 1.0.
 *
 * Since:  2.4
 **/
double
rgb_luminance (const RGB *rgb)
{
    double luminance;
    
    luminance = RGB_LUMINANCE (rgb->r, rgb->g, rgb->b);
    
    return CLAMP (luminance, 0.0, 1.0);
}

/**
 * rgb_luminance_uchar:
 * @rgb:
 *
 * Return value: the luminous intensity in the range from 0 to 255.
 *
 * Since:  2.4
 **/
unsigned char
rgb_luminance_uchar (const RGB *rgb)
{
    return ROUND (rgb_luminance (rgb) * 255.0);
}

/**
 * rgb_intensity:
 * @rgb:
 *
 * This function is deprecated! Use rgb_luminance() instead.
 *
 * Return value: the intensity in the range from 0.0 to 1.0.
 **/
double
rgb_intensity (const RGB *rgb)
{
    double intensity;
    
    intensity = RGB_INTENSITY (rgb->r, rgb->g, rgb->b);
    
    return CLAMP (intensity, 0.0, 1.0);
}

/**
 * rgb_intensity_uchar:
 * @rgb:
 *
 * This function is deprecated! Use rgb_luminance_uchar() instead.
 *
 * Return value: the intensity in the range from 0 to 255.
 **/
unsigned char
rgb_intensity_uchar (const RGB *rgb)
{
    return ROUND (rgb_intensity (rgb) * 255.0);
}

void
rgb_composite (RGB              *color1,
               const RGB        *color2,
               RGBCompositeMode  mode)
{
    switch (mode)
    {
        case RGB_COMPOSITE_NONE:
            break;
            
        case RGB_COMPOSITE_NORMAL:
            /*  put color2 on top of color1  */
            if (color2->a == 1.0)
            {
                *color1 = *color2;
            }
            else
            {
                double factor = color1->a * (1.0 - color2->a);
                
                color1->r = color1->r * factor + color2->r * color2->a;
                color1->g = color1->g * factor + color2->g * color2->a;
                color1->b = color1->b * factor + color2->b * color2->a;
                color1->a = factor + color2->a;
            }
            break;
            
        case RGB_COMPOSITE_BEHIND:
            /*  put color2 below color1  */
            if (color1->a < 1.0)
            {
                double factor = color2->a * (1.0 - color1->a);
                
                color1->r = color2->r * factor + color1->r * color1->a;
                color1->g = color2->g * factor + color1->g * color1->a;
                color1->b = color2->b * factor + color1->b * color1->a;
                color1->a = factor + color1->a;
            }
            break;
    }
}

/*  RGBA functions  */

/**
 * rgba_set:
 * @rgba: a #RGB struct
 * @red:
 * @green:
 * @blue:
 * @alpha:
 *
 * Sets the red, green, blue and alpha components of @rgb. The values
 * should be between 0.0 and 1.0 but there is no check to enforce this
 * and the values are set exactly as they are passed in.
 **/
void
rgba_set (RGB *rgba,
          double  r,
          double  g,
          double  b,
          double  a)
{
    rgba->r = r;
    rgba->g = g;
    rgba->b = b;
    rgba->a = a;
}

/**
 * rgba_set_uchar:
 * @rgba: a #RGB struct
 * @red:
 * @green:
 * @blue:
 * @alpha:
 *
 * Sets the red, green, blue and alpha components of @rgb from 8bit
 * values (0 to 255).
 **/
void
rgba_set_uchar (RGB *rgba,
                unsigned char   r,
                unsigned char   g,
                unsigned char   b,
                unsigned char   a)
{
    rgba->r = (double) r / 255.0;
    rgba->g = (double) g / 255.0;
    rgba->b = (double) b / 255.0;
    rgba->a = (double) a / 255.0;
}

void
rgba_get_uchar (const RGB *rgba,
                unsigned char        *r,
                unsigned char        *g,
                unsigned char        *b,
                unsigned char        *a)
{
    if (r) *r = ROUND (CLAMP (rgba->r, 0.0, 1.0) * 255.0);
    if (g) *g = ROUND (CLAMP (rgba->g, 0.0, 1.0) * 255.0);
    if (b) *b = ROUND (CLAMP (rgba->b, 0.0, 1.0) * 255.0);
    if (a) *a = ROUND (CLAMP (rgba->a, 0.0, 1.0) * 255.0);
}

void
rgba_add (RGB       *rgba1,
          const RGB *rgba2)
{
    rgba1->r += rgba2->r;
    rgba1->g += rgba2->g;
    rgba1->b += rgba2->b;
    rgba1->a += rgba2->a;
}

void
rgba_subtract (RGB       *rgba1,
               const RGB *rgba2)
{
    rgba1->r -= rgba2->r;
    rgba1->g -= rgba2->g;
    rgba1->b -= rgba2->b;
    rgba1->a -= rgba2->a;
}

void
rgba_multiply (RGB *rgba,
               double  factor)
{
    rgba->r *= factor;
    rgba->g *= factor;
    rgba->b *= factor;
    rgba->a *= factor;
}

double
rgba_distance (const RGB *rgba1,
               const RGB *rgba2)
{
    return (fabs (rgba1->r - rgba2->r) +
            fabs (rgba1->g - rgba2->g) +
            fabs (rgba1->b - rgba2->b) +
            fabs (rgba1->a - rgba2->a));
}
