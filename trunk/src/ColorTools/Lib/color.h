

#ifndef __COLOR_H__
#define __COLOR_H__


#include "colortypes.h"

#include "colorspace.h"

#include "rgb.h"

#if WIN32
enum {
	BLUE_PIX, 
	GREEN_PIX, 
	RED_PIX, 
	ALPHA_PIX, 
};
#else
enum {
	ALPHA_PIX, 
	RED_PIX, 
	GREEN_PIX, 
	BLUE_PIX, 
};
#endif  /* WIN32 */

#endif  /* __COLOR_H__ */
