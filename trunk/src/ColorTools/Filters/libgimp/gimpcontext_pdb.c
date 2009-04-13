#include "config.h"

#include <libgimp/gimp.h>

gboolean             gimp_context_get_foreground     (GimpRGB                *foreground)
{
    foreground->r = foreground->g = foreground->b = 0.0;
    return TRUE;
}

gboolean             gimp_context_get_background     (GimpRGB                *background)
{
    background->r = background->g = background->b = 1.0;
    return TRUE;
}

