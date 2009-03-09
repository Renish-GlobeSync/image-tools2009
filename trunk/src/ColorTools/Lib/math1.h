
#ifndef __MATH1_H__
#define __MATH1_H__

#include <math.h>
#include <float.h>

#ifdef HAVE_RINT
#define RINT(x) rint(x)
#else
#define RINT(x) floor ((x) + 0.5)
#endif

#define ROUND(x) ((int) ((x) + 0.5))

/* Square */
#define SQR(x) ((x) * (x))

#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) < (b) ? (a) : (b)

/* Limit a (0->511) int to 255 */
#define MAX255(a)  ((a) | (((a) & 256) - (((a) & 256) >> 8)))

#define CLAMP(x, min, max) (x < min) ? min : ((x > max) ?  max : x)

/* Clamp a >>int32<<-range int between 0 and 255 inclusive */
#define CLAMP0255(a)  CLAMP(a,0,255)

#define deg_to_rad(angle) ((angle) * (2.0 * G_PI) / 360.0)
#define rad_to_deg(angle) ((angle) * 360.0 / (2.0 * G_PI))

#define G_PI_4  0.78539816339744830961566084581987572104929234984378

#endif /* __MATH_H__ */
