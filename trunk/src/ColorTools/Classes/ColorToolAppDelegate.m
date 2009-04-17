//
//  ColorToolAppDelegate.m
//  ColorTool
//
//  Created by Engineer on 09-3-9.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "ColorToolAppDelegate.h"
#import "ColorToolViewController.h"
//
#import "base_enums.h"
#import "operationcolorbalance.h"
#import "operationhuesaturation.h"
#import "operationcolorize.h"
#import "operationthreshold.h"
#import "operationlevels.h"
#import "operationbrightnesscontrast.h"
#import "operationcurves.h"
#import "operationposterize.h"
//

@implementation ColorToolAppDelegate

@synthesize window;
@synthesize tabbarController;
//@synthesize nav;
//@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch
    //nav = [[UINavigationController alloc] initWithRootViewController:viewController];
    [window addSubview:tabbarController.view];
    [window makeKeyAndVisible];
}
/*
UIImage * ImageTools(UIImage * image, 
					 int (* operation)(void * in_buf, void * out_buf, long samples, void * config), 
					 void * config)
{
	CGSize imageSize = image.size;
	size_t width = imageSize.width;
	size_t height = imageSize.height;
	void * data = 0;
	//size_t const bitPerPerComponent = sizeof(float) * 8;
	size_t const bitPerPerComponent = sizeof(char) * 8;
	size_t bytesPerRow = width * (bitPerPerComponent * 4 / 8);
	CGColorSpaceRef colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	//CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
	CGContextRef context;
	if (colorspace == NULL)
		return NULL;
	data = malloc(bytesPerRow * height);
	
	context = CGBitmapContextCreate(data, width, height, bitPerPerComponent, bytesPerRow, colorspace, 
									kCGImageAlphaNoneSkipFirst);
	
	CGRect rect = {{0, 0}, {width, height}};
	CGContextDrawImage(context, rect, image.CGImage);
	
	operation(data, data, width * height, config);
	
	CGImageRef cgImage = CGBitmapContextCreateImage(context);
	return [[UIImage alloc] initWithCGImage: cgImage];
	
}
*/
- (void)dealloc {
	//[nav release];
    //[viewController release];
	[tabbarController release];
    [window release];
    [super dealloc];
}


@end
