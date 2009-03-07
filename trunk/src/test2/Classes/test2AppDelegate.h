//
//  test2AppDelegate.h
//  test2
//
//  Created by SUKEY on 09-3-3.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "base_enums.h"
#import "operationcolorbalance.h"
#import "operationhuesaturation.h"
#import "operationcolorize.h"
#import "operationthreshold.h"
#import "operationlevels.h"
#import "operationbrightnesscontrast.h"
#import "operationcurves.h"
#import "operationposterize.h"

@interface ImageFunView : UIView
{
    UIImage *image;
    color_balance_config cb_config;
	hue_saturation_config hs_config;
	colorize_config c_config;
	threshold_config t_config;
	levels_config l_config;
	brightness_contrast_config bc_config;
	curves_config cv_config;
	posterize_config p_config;
	
}
- (void)drawRect:(CGRect)rect;
@end

@interface test2AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	ImageFunView *mainView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

