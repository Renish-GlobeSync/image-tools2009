/*
 *  operationposterize.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct {
    double levels;
} posterize_config;

bool operation_posterize (void * in_buf, void * out_buf, long samples, const posterize_config * config);
