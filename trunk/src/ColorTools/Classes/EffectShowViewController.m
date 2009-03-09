//
//  EffectShowViewController.m
//  ColorTool
//
//  Created by Engineer on 09-3-9.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "EffectShowViewController.h"
#import  "ColorToolAppDelegate.h"
#import "curve.h"


@implementation EffectShowViewController
@synthesize ivImage,tvDesp;
//@synthesize colorToolType;
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
- (void)setupWithType:(int)stype{
	//colorToolType = [[NSString alloc] initWithString:stype];
	colorToolType= stype;
	//self.navigationItem.title = colorToolType;
}

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
	[self applyColorTool];
}

- (void)applyColorTool{
	UIImage * imageNew;
	switch (colorToolType) {
			
		case 0:
			self.title = @"Original Image";
			break;
		case 1:
			//NSLog(@"Color Balance");
			self.title = @"Color Balance";
			for (int i = 0; i < 3; i++) {
				cb_config.cyan_red[i] = 1.0;
				cb_config.magenta_green[i] = 0;
				cb_config.yellow_blue[i] = 0;
			}
			//
			imageNew = ImageTools(originalImage, operation_color_balance, &cb_config);
			//[delegate ImageTools
			ivImage.image = imageNew;
			break;
		case 2:
			self.title = @"Hue Saturation";
			for (int i = 0; i < 3; i++) {
				hs_config.hue[i] = -1.0;
				hs_config.saturation[i] = -1.0;
				hs_config.lightness[i] = -1.0;
			}
			imageNew = ImageTools(originalImage, operation_hue_saturation, &hs_config);
			//[delegate ImageTools
			ivImage.image = imageNew;
			break;
		case 3:
			self.title = @"Colorize";
			c_config.hue = 0.5;
			c_config.saturation = 1.0;
			c_config.lightness = .5;
			imageNew = ImageTools(originalImage, operation_colorize, &c_config);
			ivImage.image = imageNew;
			break;
		case 4:
			self.title = @"Brightness-Contrast tool";
			/*
			t_config.low = 0;
			t_config.high = 0;
			 */
			bc_config.brightness = -127.0;
			bc_config.contrast = 0.0;
			imageNew = ImageTools(originalImage, operation_brightness_contrast, &bc_config);
			ivImage.image = imageNew;
			break;
		case 5:
			self.title = @"Threshold";
		
			t_config.low = 0;
			t_config.high = 0;
			imageNew = ImageTools(originalImage, operation_threshold, &t_config);
			ivImage.image = imageNew;
			break;
		case 6:
			self.title = @"Levels";
			
			 for (int i = 0; i < 5; i++) {
			 l_config.gamma[i] = 1.0;
			 l_config.low_input[i] = 0.0;
			 l_config.high_input[i] = 1.0;
			 l_config.low_output[i] = 0.0;
			 l_config.high_output[i] = 1.0;
			 }
			imageNew = ImageTools(originalImage, operation_levels, &l_config);
			ivImage.image = imageNew;
			break;
		case 7:
			self.title = @"Curves";
			/*
			cv_config
			imageNew = ImageTools(originalImage, operation_curves, &cv_config);
			ivImage.image = imageNew;
			 */
			
			for (int i = 0; i < 5; i++) {
				cv_config.curve[i] = malloc(sizeof(*cv_config.curve[i]));
				curve_init(cv_config.curve[i]);
				curve_set_n_samples(cv_config.curve[i], 256);
				curve_set_n_points(cv_config.curve[i], 4);
			}
			
			Curve * curve = cv_config.curve[0];			
			
			curve_set_point(curve, 1, 0.25, 0.5);
			curve_set_point(curve, 1, 0.75, 0.5);
			
			curve_dirty(curve);
			
			imageNew = ImageTools(originalImage, operation_curves, &cv_config);
			ivImage.image = imageNew;
			 
			break;
		case 8:
			self.title = @"Posterize";
			p_config.levels = 2;
			imageNew = ImageTools(originalImage, operation_posterize, &p_config);
			ivImage.image = imageNew;
			break;
		default:
			break;
	}
}


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
	return [[[UIImage alloc] initWithCGImage: cgImage] autorelease];
	
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
	//
	//[colorToolType release];
	[ivImage release];
	[tvDesp release];
	//
    [super dealloc];
}


@end
