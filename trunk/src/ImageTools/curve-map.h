
#ifndef __CURVE_MAP_H__
#define __CURVE_MAP_H__


double         curve_map_value         (Curve     *curve,
										double        value);
void            curve_map_pixels        (Curve     *curve_colors,
										 Curve     *curve_red,
										 Curve     *curve_green,
										 Curve     *curve_blue,
										 Curve     *curve_alpha,
										 unsigned char        *src,
										 unsigned char        *dest,
										 long          samples);


#endif /* __CURVE_MAP_H__ */
