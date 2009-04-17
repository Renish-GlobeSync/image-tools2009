//
//  ColorToolAppDelegate.h
//  ColorTool
//
//  Created by Engineer on 09-3-9.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ColorToolViewController;

@interface ColorToolAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	//UINavigationController *nav;
    //ColorToolViewController *viewController;
	UITabBarController *tabbarController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UITabBarController *tabbarController;
//@property (nonatomic,retain) UINavigationController *nav;
//@property (nonatomic, retain) IBOutlet ColorToolViewController *viewController;
//
/*
UIImage * ImageTools(UIImage * image, 
					 int (* operation)(void * in_buf, void * out_buf, long samples, void * config), 
					 void * config);
 */
//
@end

