//
//  FiltersShowViewController.m
//  ColorTool
//
//  Created by Engineer on 09-4-17.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FiltersShowViewController.h"


@implementation FiltersShowViewController

/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	//
	UIImage *image = [UIImage imageNamed:@"sample.png"];
	//CGImageRef ref = [image CGImage];
	UILabel *lb;
	lb = [[UILabel alloc] initWithFrame:CGRectMake(10, 10, 300, 16)];
	lb.font = [UIFont boldSystemFontOfSize:14];
	lb.text = @"Oringinal Image:";
	[self.view addSubview:lb];
	[lb release];
	UIImageView *ivOrigin = [[UIImageView alloc] initWithFrame:CGRectMake(80, 40, image.size.width, image.size.height)];
	ivOrigin.image = image;
	[self.view addSubview:ivOrigin];
	[ivOrigin release];
	lb = [[UILabel alloc] initWithFrame:CGRectMake(10, image.size.height+60, 300, 16)];
	lb.font = [UIFont boldSystemFontOfSize:14];
	lb.text = @"After applied:";
	[self.view addSubview:lb];
	[lb release];
	ivImage = [[UIImageView alloc] initWithFrame:CGRectMake(80, image.size.height+90, image.size.width, image.size.height)];
	[self.view addSubview:ivImage];
	
	//tvDesp = [[UITextView alloc] initWithFrame:CGRectMake(0, image.size.width + 20, 320, 440-image.size.height)];
	[self.view addSubview:ivImage];
	
	ivImage.image = image;
	//
	//delegate = (ColorToolAppDelegate *)[UIApplication sharedApplication].delegate;
	originalImage = [UIImage imageNamed:@"sample.png"];
	//
	[self applyFilter];
}



/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)setupWithIndex:(int)index andFilterName:(NSString *)name{
	mIndex = index;
	if(name)
		mName = [[NSString alloc] initWithString:name];
	else
		mName = @"";
	//
	self.title = mName;
}

//
- (void)applyFilter{
	switch (mIndex) {
		case 0: //对应 FiltersViewController 中 filters 数组的索引  General Filters -- Convolution Matrix
			//
			//ivImage.image = ?
			break;
		case 1:
				//
			break;

		default:
			break;
	}
}
//
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

//
- (void)dealloc {
	if(mName)
		[mName release];
    [super dealloc];
}


@end
