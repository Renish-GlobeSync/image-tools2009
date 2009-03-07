//
//  test2AppDelegate.m
//  test2
//
//  Created by SUKEY on 09-3-3.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "test2AppDelegate.h"

@implementation ImageFunView

- (id)initWithFrame:(CGRect) rect {
    self = [ super initWithFrame: rect ];
    if (self != nil) {
        NSLog(@"Loading image");
        NSURL *url = [ NSURL URLWithString:
					  @"http://images.apple.com/startpage/images/2009/02/promo-safari-20090224.jpg" ];
					  //@"ftp://upload:upload@192.168.1.101/untitled.bmp" ];
        image = [ [ UIImage alloc ] initWithData:
				 [ NSData dataWithContentsOfURL: url ]
				 ];
		
		for (int i = 0; i < 3; i++) {
			cb_config.cyan_red[i] = -1.0;
			cb_config.magenta_green[i] = 0;
			cb_config.yellow_blue[i] = 0;
		}

		for (int i = 0; i < 3; i++) {
			hs_config.hue[i] = -1.0;
			hs_config.saturation[i] = -1.0;
			hs_config.lightness[i] = -1.0;
		}
		
		c_config.hue = 0;
		c_config.saturation = 0;
		c_config.lightness = 0;
		
		t_config.low = 0;
		t_config.high = 0;

		for (int i = 0; i < 5; i++) {
			l_config.gamma[i] = 1.0;
			l_config.low_input[i] = 0.0;
			l_config.high_input[i] = 1.0;
			l_config.low_output[i] = 0.0;
			l_config.high_output[i] = 1.0;
		}
		
		bc_config.brightness = -127.0;
		bc_config.contrast = 0.0;
		
		//cv_config;
		
		p_config.levels = 2;
	}
	
    return self;
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
	return [[UIImage alloc] initWithCGImage: cgImage];
	
}

- (void)drawRect:(CGRect)rect {
    CGRect myRect;
	
    myRect.size = image.size;
    myRect.origin.x = rect.origin.x + (rect.size.width  - image.size.width) / 2;
    myRect.origin.y = rect.origin.y + (rect.size.height - image.size.height) / 2;
	
	UIImage * imageNew = ImageTools(image, operation_posterize, &p_config);

    [ imageNew drawInRect: myRect];
	
    [ NSTimer scheduledTimerWithTimeInterval: 0.1
													   target: self
													 selector: @selector(handleTimer:)
													 userInfo: nil
													  repeats: NO
					  ];
}

- (void) handleTimer: (NSTimer *) timer {
    //if (cb_config.cyan_red[0] < 1.0) {
	//if (hs_config.lightness[0] < 1.0) {
	//if (c_config.saturation < 1.0) {
	//if (t_config.high < 1.0) {
	//if (l_config.low_input[0] < 1.0) {
	//if (bc_config.brightness < 127.0) {
	if (p_config.levels < 256) {
		//for (int i = 0; i < 3; i++) {
			//cb_config.cyan_red[i] += 0.05;
		//for (int i = 0; i < 7; i++) {
			//hs_config.lightness[i] += 0.05;
		//}
		//c_config.saturation += 0.05;
		//t_config.high += 0.05;
		//l_config.low_input[0] += 0.05;
		//bc_config.brightness += 1.0;
		p_config.levels += 1.0;
        [ self setNeedsDisplay ];
    }
}

- (void)dealloc {
    [ super dealloc ];
}
@end

@implementation test2AppDelegate

@synthesize window;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    

    CGRect screenBounds = [ [ UIScreen mainScreen ] applicationFrame ];
    CGRect viewRect = screenBounds;
    viewRect.origin.x = viewRect.origin.y = 0;
	
    self.window = [ [ [ UIWindow alloc ] initWithFrame: screenBounds ]
				   autorelease
				   ];
	
    mainView = [ [ ImageFunView alloc ] initWithFrame: viewRect ];
	
    [ window addSubview: mainView ];
    // Override point for customization after application launch
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [ mainView release ];
    [window release];
    [super dealloc];
}


@end
