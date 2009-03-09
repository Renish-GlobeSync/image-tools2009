
#include "color.h"
#include "math1.h"
#include "base_enums.h"
#include "operationlevels.h"

static inline double
levels_map (double value,
			double inv_gamma,
			double low_input,
			double high_input,
			double low_output,
			double high_output)
{
	/*  determine input intensity  */
	if (high_input != low_input)
		value = (value - low_input) / (high_input - low_input);
	else
		value = (value - low_input);
	
	if (inv_gamma != 1.0)
    {
		if (value >= 0.0)
			value =  pow ( value, inv_gamma);
		else
			value = -pow (-value, inv_gamma);
    }
	
	/*  determine the output intensity  */
	if (high_output >= low_output)
		value = value * (high_output - low_output) + low_output;
	else if (high_output < low_output)
		value = low_output - value * (low_output - high_output);
	
	return value;
}

typedef levels_config Config;

bool
operation_levels (void                *in_buf,
		void                *out_buf,
		long                samples,
		const Config        *config)
{
	unsigned char             *src    = in_buf;
	unsigned char             *dest   = out_buf;
	double                    inv_gamma[5];
	int                       channel;
	
	if (! config)
		return FALSE;
	
	for (channel = 0; channel < 5; channel++) {
		if (config->gamma[channel] == 0.0)
			return FALSE;
		inv_gamma[channel] = 1.0 / config->gamma[channel];
    }
	
	while (samples--)
    {
		for (channel = 0; channel < 4; channel++)
        {
			double value;
			
			value = levels_map (src[channel],
								inv_gamma[channel + 1],
								config->low_input[channel + 1],
								config->high_input[channel + 1],
								config->low_output[channel + 1],
								config->high_output[channel + 1]);
			
			/* don't apply the overall curve to the alpha channel */
			if (channel != ALPHA_PIX)
				value = levels_map (value,
									inv_gamma[0],
									config->low_input[0],
									config->high_input[0],
									config->low_output[0],
									config->high_output[0]);
			
			dest[channel] = value;
        }
		
		src  += 4;
		dest += 4;
    }
	
	return TRUE;
}


/*  public functions  */

double
levels_map_input (const Config         *config,
				  HistogramChannel     channel,
				  double               value)
{
	/*  determine input intensity  */
	if (config->high_input[channel] != config->low_input[channel])
		value = ((value - config->low_input[channel]) /
				 (config->high_input[channel] - config->low_input[channel]));
	else
		value = (value - config->low_input[channel]);
	
	value = CLAMP (value, 0.0, 1.0);
	
	if (config->gamma[channel] != 0.0)
    {
		value = pow (value, 1.0 / config->gamma[channel]);
    }
	
	return value;
}
