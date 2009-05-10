
#ifndef __COLOR_SPACE_H__
#define __COLOR_SPACE_H__

/* For information look into the C source or the html documentation */


/*  Color conversion routines  */


/*  RGB function  */

void   rgb_to_hsv          (const RGB  *rgb,
                            HSV        *hsv);
void   rgb_to_hsl          (const RGB  *rgb,
                            HSL        *hsl);
void   rgb_to_cmyk         (const RGB  *rgb,
                            double         pullout,
                            CMYK       *cmyk);

void   hsv_to_rgb          (const HSV  *hsv,
                            RGB        *rgb);
void   hsl_to_rgb          (const HSL  *hsl,
                            RGB        *rgb);
void   cmyk_to_rgb         (const CMYK *cmyk,
                            RGB        *rgb);

void   rgb_to_hwb          (const RGB  *rgb,
                            double        *hue,
                            double        *whiteness,
                            double        *blackness);
void   hwb_to_rgb          (double         hue,
                            double         whiteness,
                            double         blackness,
                            RGB        *rgb);


/*  int functions  */

void    rgb_to_hsv_int     (int    *red         /* returns hue        */,
                            int    *green       /* returns saturation */,
                            int    *blue        /* returns value      */);
void    hsv_to_rgb_int     (int    *hue         /* returns red        */,
                            int    *saturation  /* returns green      */,
                            int    *value       /* returns blue       */);

void    rgb_to_cmyk_int    (int    *red         /* returns cyan       */,
                            int    *green       /* returns magenta    */,
                            int    *blue        /* returns yellow     */,
                            int    *pullout     /* returns black      */);
void    cmyk_to_rgb_int    (int    *cyan        /* returns red        */,
                            int    *magenta     /* returns green      */,
                            int    *yellow      /* returns blue       */,
                            int    *black       /* not changed        */);

void    rgb_to_hsl_int     (int    *red         /* returns hue        */,
                            int    *green       /* returns saturation */,
                            int    *blue        /* returns lightness  */);
int    rgb_to_l_int       (int     red,
                            int     green,
                            int     blue);
void    hsl_to_rgb_int     (int    *hue         /* returns red        */,
                            int    *saturation  /* returns green      */,
                            int    *lightness   /* returns blue       */);


/*  double functions  */

void    rgb_to_hsv4        (const unsigned char *rgb,
                            double      *hue,
                            double      *saturation,
                            double      *value);
void    hsv_to_rgb4        (unsigned char       *rgb,
                            double       hue,
                            double       saturation,
                            double       value);

#endif  /* __COLOR_SPACE_H__ */
