//
//  AppDelegate.m
//  FeatureLab
//
//  Created by Kai Oezer on 6/14/18.
//  Copyright © 2018 Kai Oezer. All rights reserved.
//

#import "FLAppDelegate.h"

@implementation FLAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleWindowWillClose:) name:NSWindowWillCloseNotification object:nil];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
  // Insert code here to tear down your application
}

- (void) handleWindowWillClose:(NSNotification*)notification
{
  [NSApp terminate:self];
}

@end
