/*
 *  levels.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct {
	double               gamma[5];
	double               low_input[5];
	double               high_input[5];
	double               low_output[5];
	double               high_output[5];	
} levels_config;

bool operation_levels (void * in_buf, void * out_buf, long samples, const levels_config * config);
