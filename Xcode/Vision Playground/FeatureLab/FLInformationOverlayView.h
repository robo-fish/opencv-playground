//
//  FLInformationOverlayView.h
//  FeatureLab
//
//  Created by Kai Oezer on 6/16/18.
//  Copyright © 2018 Kai Oezer. All rights reserved.
//

#import <AppKit/AppKit.h>
#include <vector>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/core.hpp>
#pragma GCC diagnostic pop

@interface FLInformationOverlayView : NSView

- (void) setKeyPoints:(std::vector<cv::KeyPoint> &&)keyPoints
      imageDimensions:(CGSize)dimensions;

- (void) clearPoints;

- (void) showMessage:(NSString*)message;
- (void) clearMessage;

@end
