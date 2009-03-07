/*
 *  image_tools.h
 *  test1
 *
 *  Created by SUKEY on 09-3-3.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

int ImageTools_ColorBalance_Float(void * data, int width, int heigth, int preserve_lum, double cyan_red, double mnagenta_green, double yellow_blue);
int ImageTools_ColorBalance(void * data, int width, int height, int preserve_lum, int cyan_red, int mnagenta_green, int yellow_blue);
