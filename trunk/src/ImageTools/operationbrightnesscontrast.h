/*
 *  brightnesscontrast.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct {
	double             brightness;
	double             contrast;
} brightness_contrast_config;

bool operation_brightness_contrast (void * in_buf, void * out_buf, long samples, const brightness_contrast_config * config);
