//
//  FLImageUtilities.h
//  FeatureLab
//
//  Created by Kai Oezer on 6/19/18.
//  Copyright © 2018 Kai Oezer. All rights reserved.
//

#import <AppKit/AppKit.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/core.hpp>
#pragma GCC diagnostic pop

@interface FLImageUtilities : NSObject

+ (CGImageRef) createCGImageFromNSImage:(NSImage*)image;

+ (NSImage*) imageFromMat:(cv::Mat)cvMat;

+ (cv::Mat) matFromCgImage:(CGImageRef)image;

@end
