//
//  FiltersShowViewController.h
//  ColorTool
//
//  Created by Engineer on 09-4-17.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface FiltersShowViewController : UIViewController {
	int mIndex;
	NSString *mName;
	//
	UIImageView *ivImage;
	UIImage *originalImage;
}
- (void)applyFilter;
@end
