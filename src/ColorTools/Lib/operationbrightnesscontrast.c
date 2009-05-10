/*
*  brightnesscontrast.c
*  test2
*
*  Created by SUKEY on 09-3-5.
*  Copyright 2009 __MyCompanyName__. All rights reserved.
*
*/

#include "color.h"
#include "math1.h"
#include "base_enums.h"
#include "operationbrightnesscontrast.h"

#include "operationlevels.h"

typedef brightness_contrast_config Config;

static bool
brightness_contrast_config_to_levels_config (levels_config *levels, 
                                             const Config  *config)
{
    double           brightness;
    double           slant;
    double           value;

    brightness = config->brightness / 2.0;
    slant = tan ((config->contrast + 1) * G_PI_4);

    if (config->brightness >= 0)
    {
        value = -0.5 * slant + brightness * slant + 0.5;

        if (value < 0.0)
        {
            value = 0.0;

            /* this slightly convoluted math follows by inverting the
            * calculation of the brightness/contrast LUT in base/lut-funcs.h */

            levels->low_input[HISTOGRAM_VALUE] =
                (- brightness * slant + 0.5 * slant - 0.5) / (slant - brightness * slant);
        }

        levels->low_output[HISTOGRAM_VALUE] = value;

        value = 0.5 * slant + 0.5;

        if (value > 1.0)
        {
            value = 1.0;

            levels->high_input[HISTOGRAM_VALUE] =
                (- brightness * slant + 0.5 * slant + 0.5) / (slant - brightness * slant);
        }

        levels->high_output[HISTOGRAM_VALUE] = value;
    }
    else
    {
        value = 0.5 - 0.5 * slant;

        if (value < 0.0)
        {
            value = 0.0;

            levels->low_input[HISTOGRAM_VALUE] =
                (0.5 * slant - 0.5) / (slant + brightness * slant);
        }

        levels->low_output[HISTOGRAM_VALUE] = value;

        value = slant * brightness + slant * 0.5 + 0.5;

        if (value > 1.0)
        {
            value = 1.0;

            levels->high_input[HISTOGRAM_VALUE] =
                (0.5 * slant + 0.5) / (slant + brightness * slant);
        }

        levels->high_output[HISTOGRAM_VALUE] = value;
    }

    return TRUE;
}

bool
operation_brightness_contrast (void                 *in_buf, 
                               void                 *out_buf, 
                               long                 samples, 
                               const Config         *config)
{
    int i;
    levels_config l_config;
    for (i = 0; i < 5; i++) {
        l_config.gamma[i] = 1.0;
        l_config.low_input[i] = 0.0;
        l_config.high_input[i] = 1.0;
        l_config.low_output[i] = 0.0;
        l_config.high_output[i] = 1.0;
    }
    brightness_contrast_config_to_levels_config(&l_config, config);
    return operation_levels(in_buf, out_buf, samples, &l_config);
}

//bool
//operation_brightness_contrast_optimize (void                 *in_buf, 
//                                        void                 *out_buf, 
//                                        long                 samples, 
//                                        const Config         *config)
//{
//    int i;
//    levels_config l_config;
//    for (i = 0; i < 5; i++) {
//        l_config.gamma[i] = 1.0;
//        l_config.low_input[i] = 0.0;
//        l_config.high_input[i] = 1.0;
//        l_config.low_output[i] = 0.0;
//        l_config.high_output[i] = 1.0;
//    }
//    brightness_contrast_config_to_levels_config(&l_config, config);
//    return operation_levels_optimize (in_buf, out_buf, samples, &l_config);
//}
