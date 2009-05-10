/*
 *  base_enums.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __BASE_ENUMS__
#define __BASE_ENUMS__

typedef enum {
    SHADOWS, 
    MIDTONES, 
    HIGHLIGHTS
} TransferMode;

typedef enum {
    HISTOGRAM_VALUE = 0,  /*< desc="Value" >*/
    HISTOGRAM_RED   = 1,  /*< desc="Red"   >*/
    HISTOGRAM_GREEN = 2,  /*< desc="Green" >*/
    HISTOGRAM_BLUE  = 3,  /*< desc="Blue"  >*/
    HISTOGRAM_ALPHA = 4,  /*< desc="Alpha" >*/
    HISTOGRAM_RGB   = 5   /*< desc="RGB", pdb-skip >*/
} HistogramChannel;

typedef enum {
    ALL_HUES, 
    RED_HUES, 
    YELLOW_HUES, 
    GREEN_HUES, 
    CYAN_HUES, 
    BLUE_HUES, 
    MAGENTA_HUES
} HueRange;

typedef enum {
    CURVE_SMOOTH,   /*< desc="Smooth"   >*/
    CURVE_FREE      /*< desc="Freehand" >*/
} CurveType;


#endif // __BASE_ENUMS__
