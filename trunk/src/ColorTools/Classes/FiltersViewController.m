//
//  FiltersViewController.m
//  ColorTool
//
//  Created by Engineer on 09-4-17.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FiltersViewController.h"
#import "FiltersShowViewController.h"


@implementation FiltersViewController

/*
- (id)initWithStyle:(UITableViewStyle)style {
    // Override initWithStyle: if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
    if (self = [super initWithStyle:style]) {
    }
    return self;
}
*/


- (void)viewDidLoad {
    [super viewDidLoad];

    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
	self.title = @"Filters";
	/*
	 General Filters: (3) 
	 1. Convolution Matrix 
	 2. Dilate 
	 3. Erode 
	 
	 Enhance Filters: (5)
	 １。　Deinterface
	 2.  NL Filter 
	 3.  Red Eye Removal 
	 4. UnSharpen  Mask
	 5.  Sharpen 
	 
	 Light and Shadow Filter:  (3)
	 
	 1.   Lens Flare
	 2.  Supernova
	 3.  Apply Lens
	 
	 Distort Filters:   (3)
	 1. Page Curl
	 2. Lens Distortion
	 3.  Whirl and Pinch
	 
	 Artistic Filter:  (3)
	 1. Cartoon 
	 2.  Soft Glow 
	 3. Oilify
	 */
	filters = [[NSMutableArray alloc] init];
	[filters addObject:@"General -- Convolution Matrix"];
	[filters addObject:@"General -- Dilate"];
	[filters addObject:@"General -- Erode"];
	//
	[filters addObject:@"Enhance -- Deinterlace"];
	[filters addObject:@"Enhance -- NL Filter"];
	[filters addObject:@"Enhance -- Red Eye Removal"];
	[filters addObject:@"Enhance -- UnSharpen  Mask"];
	[filters addObject:@"Enhance -- Sharpen"];
	//
	[filters addObject:@"Light and Shadow -- Lens Flare"];
	[filters addObject:@"Light and Shadow -- Supernova"];
	[filters addObject:@"Light and Shadow -- Apply Lens"];
	//
	[filters addObject:@"Distort -- Page Curl"];
	[filters addObject:@"Distort -- Lens Distortion"];
	[filters addObject:@"Distort -- Whirl and Pinch"];
	//
	[filters addObject:@"Artistic -- Cartoon"];
	[filters addObject:@"Artistic -- Soft Glow"];
	[filters addObject:@"Artistic -- Oilify"];
}


/*
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}
*/
/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/
/*
- (void)viewWillDisappear:(BOOL)animated {
	[super viewWillDisappear:animated];
}
*/
/*
- (void)viewDidDisappear:(BOOL)animated {
	[super viewDidDisappear:animated];
}
*/

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

#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}


// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [filters count];
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
    }
    
	cell.text  = [filters objectAtIndex:indexPath.row];
    // Set up the cell...

    return cell;
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    // Navigation logic may go here. Create and push another view controller.
	// AnotherViewController *anotherViewController = [[AnotherViewController alloc] initWithNibName:@"AnotherView" bundle:nil];
	// [self.navigationController pushViewController:anotherViewController];
	// [anotherViewController release];
	FiltersShowViewController *vc = [[FiltersShowViewController alloc] init];
	[vc setupWithIndex:indexPath.row andFilterName:[filters objectAtIndex:indexPath.row]];
	[self.navigationController pushViewController:vc animated:YES];
	[vc release];
}


/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/


/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/


/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/


/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/


- (void)dealloc {
	[filters release];
    [super dealloc];
}


@end

