/*
 *  colorize.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct {
	double             hue;
	double             saturation;
	double             lightness;
} colorize_config;

bool operation_colorize(void * in_buf, void * out_buf, long samples, const colorize_config * config);