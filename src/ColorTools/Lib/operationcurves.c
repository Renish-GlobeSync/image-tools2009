
#include "color.h"
#include "math1.h"
#include "base_enums.h"
#include "operationcurves.h"
#include "curve-map.h"

typedef curves_config Config;

bool
operation_curves (void                *in_buf,
                               void                *out_buf,
                               long                samples,
                               const Config *config)
{
  unsigned char                   *src    = in_buf;
  unsigned char                   *dest   = out_buf;

  if (! config)
    return FALSE;

	curve_map_pixels (config->curve[0],
                         config->curve[1],
                         config->curve[2],
                         config->curve[3],
                         config->curve[4], src, dest, samples);

  return TRUE;
}
