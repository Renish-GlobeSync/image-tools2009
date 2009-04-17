
#ifndef DEFAULT_FILTER_CONFIG_H
#define DEFAULT_FILTER_CONFIG_H

#include "config.h"
	
#include <libgimp/gimp.h>

#include "convolution_matrix.h"
#include "value-propagate.h"
#include "deinterlace.h"
#include "nl-filter.h"
#include "red-eye-removal.h"
#include "sharpen.h"
#include "unsharp_mask.h"
#include "lens-flare.h"
#include "nova.h"
#include "lens-apply.h"
#include "lens-distortion.h"
#include "whirl-pinch.h"
#include "cartoon.h"
#include "softglow.h"
#include "oilify.h"
#include "pagecurl.h"

convolution_matrix_config config_convolution_matrix = {
  {
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, -1.0,1.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 }
  },                 /* matrix */
  1,                 /* divisor */
  0,                 /* offset */
  0,                 /* Alpha-handling algorithm */
  CLEAR,             /* border-mode */
  { TRUE, TRUE, TRUE, TRUE, TRUE }, /* Channels mask */
  FALSE,              /* autoset */
};

deinterlace_config config_deinterlace = {
    FALSE
};

nl_filter_config config_nl_filter = {
  1.0,
  1.0,
  2
};

red_eye_removal_config config_red_eye_removal = {
    50
};

sharpen_config config_sharpen = {
    100
};

unsharp_mask_config config_unsharp_mask = {
    120, /* default radius    */
    6, /* default amount    */
    0    /* default threshold */
};

lens_flare_config config_lens_flare = {
    20, 20
};

nova_config config_nova = {
  128, 128,                 /* xcenter, ycenter */
  { 0.35, 0.39, 1.0, 1.0 }, /* color */
  65,                       /* radius */
  467,                      /* nspoke */
  254                         /* random hue */
};

lens_apply_config config_lens_apply = {
  /* Lens refraction value */
  1.7,
  /* Surroundings options */
  TRUE, FALSE, FALSE
};

lens_distortion_config config_lens_distortion = {
    0.0, 0.0, 0.0, 200.0, 0.0, 100.0
};

whirl_pinch_config config_whirl_pinch = {
    90.0, /* whirl   */
    0.0,  /* pinch   */
    1.0   /* radius  */
};

cartoon_config config_cartoon = {
    50.0,  /* mask_radius */
    1.0,  /* threshold */
    0.2   /* pct_black */
};

softglow_config config_softglow = {
    50.0, /* glow_radius */
    0.75, /* brightness */
    0.85,  /* sharpness */
};

oilify_config config_oilify = {
    8.0,        /* mask size          */
    FALSE,      /* use mask-size map? */
    -1,         /* mask-size map      */
    8.0,        /* exponent           */
    FALSE,      /* use exponent map?  */
    -1,         /* exponent map       */
    MODE_INTEN  /* mode               */
};

pagecurl_config config_pagecurl = {
    CURL_COLORS_FG_BG, 
    1.0, 
    TRUE, 
    CURL_EDGE_LOWER_RIGHT, 
    CURL_ORIENTATION_VERTICAL
};

#endif
