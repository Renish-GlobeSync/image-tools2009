//
//  FiltersShowViewController.m
//  ColorTool
//
//  Created by Engineer on 09-4-17.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FiltersShowViewController.h"

#import "default.h"


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
	UIImage *image = [UIImage imageNamed:@"filter.png"];
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
	originalImage = [UIImage imageNamed:@"filter.png"];
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

UIImage * Filters(UIImage * image, 
				  int (* operation)(void * in_buf, void * out_buf, long width, long height, void * config), 
				  void * config)
{
	CGSize imageSize = image.size;
	size_t width = imageSize.width;
	size_t height = imageSize.height;
	void * data = 0;
	void * data1 = 0;
	//size_t const bitPerPerComponent = sizeof(float) * 8;
	size_t const bitPerPerComponent = sizeof(char) * 8;
	size_t bytesPerRow = width * (bitPerPerComponent * 4 / 8);
	CGColorSpaceRef colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	//CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
	CGContextRef context;
	if (colorspace == NULL)
		return NULL;
	data = malloc(bytesPerRow * height);
	data1 = malloc(bytesPerRow * height);
	
	context = CGBitmapContextCreate(data, width, height, bitPerPerComponent, bytesPerRow, colorspace, 
									kCGImageAlphaPremultipliedLast);
	
	CGRect rect = {{0, 0}, {width, height}};
	CGContextDrawImage(context, rect, image.CGImage);
	
	operation(data, data1, width, height, config);
	
	memcpy(data, data1, bytesPerRow * height);
	
	CGImageRef cgImage = CGBitmapContextCreateImage(context);
	return [[[UIImage alloc] initWithCGImage: cgImage] autorelease];
	
}

//
- (void)applyFilter{
	switch (mIndex) {
		case 0: //对应 FiltersViewController 中 filters 数组的索引  General Filters -- Convolution Matrix
			//
			ivImage.image = Filters(originalImage, operation_convolution_matrix, &config_convolution_matrix);
			break;
		case 1:
			ivImage.image = Filters(originalImage, operation_dilate, NULL);
			break;
		case 2:
			ivImage.image = Filters(originalImage, operation_erode, NULL);
			break;
		case 3:
			ivImage.image = Filters(originalImage, operation_deinterlace, &config_deinterlace);
			break;
		case 4:
			ivImage.image = Filters(originalImage, operation_nl_filter, &config_nl_filter);
			break;
		case 5:
			ivImage.image = Filters(originalImage, operation_red_eye_removal, &config_red_eye_removal);
			break;
		case 6:
			ivImage.image = Filters(originalImage, operation_unsharp_mask, &config_unsharp_mask);
			break;
		case 7:
			ivImage.image = Filters(originalImage, operation_sharpen, &config_sharpen);
			break;
		case 8:
			ivImage.image = Filters(originalImage, operation_lens_flare, &config_lens_flare);
			break;
		case 9:
			ivImage.image = Filters(originalImage, operation_nova, &config_nova);
			break;
		case 10:
			ivImage.image = Filters(originalImage, operation_lens_apply, &config_lens_apply);
			break;
		case 11:
			ivImage.image = Filters(originalImage, operation_pagecurl, &config_pagecurl);
			break;
		case 12:
			ivImage.image = Filters(originalImage, operation_lens_distortion, &config_lens_distortion);
			break;
		case 13:
			ivImage.image = Filters(originalImage, operation_whirl_pinch, &config_whirl_pinch);
			break;
		case 14:
			ivImage.image = Filters(originalImage, operation_cartoon, &config_cartoon);
			break;
		case 15:
			ivImage.image = Filters(originalImage, operation_cartoon, &config_cartoon);
			break;
		case 16:
			ivImage.image = Filters(originalImage, operation_softglow, &config_softglow);
			break;
		case 17:
			ivImage.image = Filters(originalImage, operation_oilify, &config_oilify);
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
