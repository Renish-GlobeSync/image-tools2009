
#ifndef __HSV_H__
#define __HSV_H__

void    hsv_set        (HSV           *hsv,
                        double        hue,
                        double        saturation,
                        double        value);
void    hsv_clamp      (HSV           *hsv);

void    hsva_set       (HSV           *hsva,
                        double        hue,
                        double        saturation,
                        double        value,
                        double        alpha);

#endif  /* __HSV_H__ */
