//
//  EffectShowViewController.h
//  ColorTool
//
//  Created by Engineer on 09-3-9.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ColorToolAppDelegate.h"
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

@interface EffectShowViewController : UIViewController {
	UIImageView *ivImage;
	UITextView *tvDesp;
	int colorToolType;
	ColorToolAppDelegate *delegate;
	UIImage *originalImage;
	//
	color_balance_config cb_config;
	hue_saturation_config hs_config;
	colorize_config c_config;
	threshold_config t_config;
	levels_config l_config;
	brightness_contrast_config bc_config;
	curves_config cv_config;
	posterize_config p_config;
	//

}
- (void)setupWithType:(int)stype;
- (void)applyColorTool;
UIImage * ImageTools(UIImage * image, 
					 int (* operation)(void * in_buf, void * out_buf, long samples, void * config), 
					 void * config);
@property(nonatomic,retain)UIImageView *ivImage;
@property(nonatomic,retain)UITextView *tvDesp;
@property(nonatomic,retain)NSString *colorToolType;
@end
