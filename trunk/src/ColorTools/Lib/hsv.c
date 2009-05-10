
#include "colortypes.h"
#include "math1.h"
#include "hsv.h"

/*  HSV functions  */

void
hsv_set (HSV     *hsv,
         double  h,
         double  s,
         double  v)
{
    
    hsv->h = h;
    hsv->s = s;
    hsv->v = v;
}

void
hsv_clamp (HSV    *hsv)
{
    hsv->h -= (int) hsv->h;
    
    if (hsv->h < 0)
        hsv->h += 1.0;
    
    hsv->s = CLAMP (hsv->s, 0.0, 1.0);
    hsv->v = CLAMP (hsv->v, 0.0, 1.0);
    hsv->a = CLAMP (hsv->a, 0.0, 1.0);
}

void
hsva_set (HSV     *hsva,
          double  h,
          double  s,
          double  v,
          double  a)
{
    hsva->h = h;
    hsva->s = s;
    hsva->v = v;
    hsva->a = a;
}
