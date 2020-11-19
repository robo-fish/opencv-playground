//
//  FLInformationOverlayView.m
//  FeatureLab
//
//  Created by Kai Oezer on 6/16/18.
//  Copyright © 2018 Kai Oezer. All rights reserved.
//

#import "FLInformationOverlayView.h"

@implementation FLInformationOverlayView
{
@private
  std::vector<cv::KeyPoint> _points;
  CGSize _dimensions;
  CGColorSpaceRef _colorspace;
  NSString* _message;
  NSDictionary<NSAttributedStringKey,id>* _messageTextAttributes;
}

- (void) setKeyPoints:(std::vector<cv::KeyPoint> &&)points imageDimensions:(CGSize)dimensions
{
  _points = points;
  _dimensions = dimensions;
  [self setNeedsDisplay:YES];
}

- (void) clearPoints
{
  _points.clear();
  [self setNeedsDisplay:YES];
}

- (void) drawRect:(CGRect)rect
{
  if (_colorspace == nil)
  {
    _colorspace = CGColorSpaceCreateDeviceRGB();
  }
  if (!_points.empty())
  {
    [self _drawFeaturePoints];
  }
  if (_message != nil)
  {
    [self _drawMessage];
  }
}

- (void) showMessage:(NSString*)message
{
  if (_message != message)
  {
    _message = message;
    [self setNeedsDisplay:YES];
  }
}

- (void) clearMessage
{
  [self showMessage:nil];
}

//MARK: -

static int kMaxNumberOfPointsToDraw = 400;

- (void) _drawFeaturePoints
{
	CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
  CGContextSaveGState(context);
  static CGFloat const sideDim = 6.0;
  static CGFloat const Pi2 = 6.28318531;
  CGSize const canvasSize = self.frame.size;
  CGFloat const factorX = canvasSize.width / _dimensions.width;
  CGFloat const factorY = canvasSize.height / _dimensions.height;
  CGFloat redColor[] = { 1.0, 0.0, 0.0, 1.0 };
  CGContextSetStrokeColorSpace(context, _colorspace);
  CGContextSetStrokeColor(context, redColor);
  CGContextSetLineWidth(context, 2.0);
  CGContextBeginPath(context);
  int pointIdx = 0;
  for (auto p : _points)
  {
    CGFloat const dim = (p.size > 0) ? p.size : sideDim;
    CGFloat const x = p.pt.x * factorX;
    CGFloat const y = canvasSize.height - p.pt.y * factorY;
    CGContextAddEllipseInRect(context, CGRectMake(x - dim, y - dim, 2 * dim, 2 * dim));
    if (p.angle >= 0)
    {
      CGContextMoveToPoint(context, x, y);
      CGContextAddLineToPoint(context, x + cos(p.angle/Pi2)*dim, y + sin(p.angle/Pi2)*dim);
    }
    if (++pointIdx >= kMaxNumberOfPointsToDraw)
    {
      break;
    }
  }
  CGContextStrokePath(context);
  CGContextRestoreGState(context);
}

- (void) _drawMessage
{
  if (_messageTextAttributes == nil)
  {
    _messageTextAttributes = @{
      NSFontAttributeName : [NSFont systemFontOfSize:24.0],
      NSForegroundColorAttributeName : [NSColor grayColor],
    };
  }
  NSAttributedString* attribString = [[NSAttributedString alloc] initWithString:_message attributes:_messageTextAttributes];
  CGSize const stringSize = [attribString size];
  CGSize const viewSize = self.frame.size;
  [attribString drawAtPoint: CGPointMake((viewSize.width - stringSize.width)/2, viewSize.height * .60)];
}

@end
