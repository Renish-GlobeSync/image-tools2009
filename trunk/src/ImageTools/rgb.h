#ifndef __RGB_H__
#define __RGB_H__

/*  RGB and RGBA color types and operations taken from LibGCK  */

typedef enum
	{
		RGB_COMPOSITE_NONE = 0,
		RGB_COMPOSITE_NORMAL,
		RGB_COMPOSITE_BEHIND
	} RGBCompositeMode;


void      rgb_set             (RGB       *rgb,
								double        red,
								double        green,
								double        blue);
void      rgb_set_alpha       (RGB       *rgb,
								double        alpha);

void      rgb_set_uchar       (RGB       *rgb,
								unsigned char         red,
								unsigned char         green,
								unsigned char         blue);
void      rgb_get_uchar       (const RGB *rgb,
								unsigned char        *red,
								unsigned char        *green,
								unsigned char        *blue);

int  rgb_parse_name      (RGB       *rgb,
								const char   *name,
								int           len);
bool  rgb_parse_hex       (RGB       *rgb,
								const char   *hex,
								int           len);
bool  rgb_parse_css       (RGB       *rgb,
								const char   *css,
								int           len);

void      rgb_add             (RGB       *rgb1,
								const RGB *rgb2);
void      rgb_subtract        (RGB       *rgb1,
								const RGB *rgb2);
void      rgb_multiply        (RGB       *rgb1,
								double        factor);
double   rgb_distance        (const RGB *rgb1,
							   const RGB *rgb2);

double   rgb_max             (const RGB *rgb);
double   rgb_min             (const RGB *rgb);
void      rgb_clamp           (RGB       *rgb);

void      rgb_gamma           (RGB       *rgb,
								double        gamma);

double   rgb_luminance       (const RGB *rgb);
unsigned char    rgb_luminance_uchar (const RGB *rgb);

double   rgb_intensity       (const RGB *rgb);
unsigned char    rgb_intensity_uchar (const RGB *rgb);

void      rgb_composite       (RGB              *color1,
								const RGB        *color2,
								RGBCompositeMode  mode);

/*  access to the list of color names  */
int      rgb_list_names      (const char ***names,
							   RGB      **colors);


void      rgba_set            (RGB       *rgba,
								double        red,
								double        green,
								double        blue,
								double        alpha);

void      rgba_set_uchar      (RGB       *rgba,
								unsigned char         red,
								unsigned char         green,
								unsigned char         blue,
								unsigned char         alpha);
void      rgba_get_uchar      (const RGB *rgba,
								unsigned char        *red,
								unsigned char        *green,
								unsigned char        *blue,
								unsigned char        *alpha);

bool  rgba_parse_css      (RGB       *rgba,
								const char   *css,
								int           len);

void      rgba_add            (RGB       *rgba1,
								const RGB *rgba2);
void      rgba_subtract       (RGB       *rgba1,
								const RGB *rgba2);
void      rgba_multiply       (RGB       *rgba,
								double        factor);

double   rgba_distance       (const RGB *rgba1,
							   const RGB *rgba2);



/*  Map RGB to intensity  */

/*
 * The weights to compute true CIE luminance from linear red, green
 * and blue, as defined by the ITU-R Recommendation BT.709, "Basic
 * Parameter Values for the HDTV Standard for the Studio and for
 * International Programme Exchange" (1990). Also suggested in the
 * sRGB colorspace specification by the W3C.
 */

#define RGB_LUMINANCE_RED    (0.2126)
#define RGB_LUMINANCE_GREEN  (0.7152)
#define RGB_LUMINANCE_BLUE   (0.0722)

#define RGB_LUMINANCE(r,g,b) ((r) * RGB_LUMINANCE_RED   + \
(g) * RGB_LUMINANCE_GREEN + \
(b) * RGB_LUMINANCE_BLUE)


/*
 * The coefficients below properly computed luminance for monitors
 * having phosphors that were contemporary at the introduction of NTSC
 * television in 1953. They are still appropriate for computing video
 * luma. However, these coefficients do not accurately compute
 * luminance for contemporary monitors. The use of these definitions
 * is deprecated.
 */

#define RGB_INTENSITY_RED    (0.30)
#define RGB_INTENSITY_GREEN  (0.59)
#define RGB_INTENSITY_BLUE   (0.11)

#define RGB_INTENSITY(r,g,b) ((r) * RGB_INTENSITY_RED   + \
(g) * RGB_INTENSITY_GREEN + \
(b) * RGB_INTENSITY_BLUE)

#endif  /* __RGB_H__ */
