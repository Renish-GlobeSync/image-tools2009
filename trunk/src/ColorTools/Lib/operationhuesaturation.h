/*
 *  huesaturation.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct _hue_saturation_config {
    double             hue[7];
    double             saturation[7];
    double             lightness[7];
    double             overlap;
} hue_saturation_config;

bool operation_hue_saturation(void * in_buf, void * out_buf, long samples, const hue_saturation_config * config);