/*
 *  corves.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "curve.h"

typedef struct {
    Curve   *curve[5];
} curves_config;

bool operation_curves (void * in_buf, void * out_buf, long samples, const curves_config * config);
