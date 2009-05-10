/*
 *  colorbalance.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct {
    bool        preserve_luminosity;
    double        cyan_red[3];
    double        magenta_green[3];
    double        yellow_blue[3];

} color_balance_config;

bool operation_color_balance(void * in_buf, void * out_buf, long samples, const color_balance_config * config);

bool operation_color_balance_optimize(void * in_buf, void * out_buf, long samples, const color_balance_config * config);
