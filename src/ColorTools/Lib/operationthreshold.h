/*
 *  threshold.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct {
    double          low;
    double          high;
} threshold_config;

bool operation_threshold(void * in_buf, void * out_buf, long samples, const threshold_config * config);
