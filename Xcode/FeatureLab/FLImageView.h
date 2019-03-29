//
//  FLImageView.h
//  FeatureLab
//
//  Created by Kai Oezer on 6/17/18.
//  Copyright © 2018 Kai Oezer. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol FLImageViewDelegate
// @return whether the file at the given location was processed.
- (BOOL) handleDroppedFileLocation:(NSURL*)fileLocation;
@end

@interface FLImageView : NSView <NSDraggingDestination>
@property id<FLImageViewDelegate> delegate;
@property (nonatomic) NSImage* image;
@end
