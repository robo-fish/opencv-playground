//
//  FLImageUtilities.m
//  FeatureLab
//
//  Created by Kai Oezer on 6/19/18.
//  Copyright © 2018 Kai Oezer unless noted otherwise. All rights reserved.
//

#import "FLImageUtilities.h"

@implementation FLImageUtilities

// by Dominic Klessel (https://gist.github.com/dominiklessel/1716068)
+ (NSImage*) imageFromMat:(cv::Mat)cvMat
{
  NSData *data = [NSData dataWithBytes:cvMat.data length:cvMat.elemSize() * cvMat.total()];
  CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);

  CGColorSpaceRef colorSpace = (cvMat.elemSize() == 1) ?  CGColorSpaceCreateDeviceGray() : CGColorSpaceCreateDeviceRGB();
  int const bitsPerComponent = 8;
  long const bitsPerPixel = bitsPerComponent * cvMat.elemSize();
  long const bytesPerRow = cvMat.step[0];
  int const alphaInfo = kCGImageAlphaNone | kCGBitmapByteOrderDefault;
  bool const interpolate = false;

  CGImageRef imageRef = CGImageCreate(cvMat.cols, cvMat.rows, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpace, alphaInfo, provider, NULL, interpolate, kCGRenderingIntentDefault);
  NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:imageRef];
  NSImage *image = [[NSImage alloc] init];
  [image addRepresentation:bitmapRep];

  CGImageRelease(imageRef);
  CGDataProviderRelease(provider);
  CGColorSpaceRelease(colorSpace);

  return image;
}

+ (CGImageRef) createCGImageFromNSImage:(NSImage*)image
{
  NSData *imageData = image.TIFFRepresentation;
  CGImageSourceRef source = CGImageSourceCreateWithData((__bridge CFDataRef)imageData, NULL);
  CGImageRef result = CGImageSourceCreateImageAtIndex(source, 0, NULL);
  assert(CGImageGetWidth(result) == image.size.width);
  assert(CGImageGetHeight(result) == image.size.height);
  return result;
}

+ (cv::Mat) matFromCgImage:(CGImageRef)image
{
  size_t const width{CGImageGetWidth(image)};
  size_t const height{CGImageGetHeight(image)};
  size_t const bpc = CGImageGetBitsPerComponent(image);
  size_t const bpp = CGImageGetBitsPerPixel(image);
  size_t const cpp = bpp / bpc;
  //CGImageAlphaInfo const alphaInfo = CGImageGetAlphaInfo(image);

  /*
    Valid parameters for RGB color space model are:

     16 bits per pixel,     5 bits per component,     kCGImageAlphaNoneSkipFirst
     32 bits per pixel,     8 bits per component,     kCGImageAlphaNoneSkipFirst
     32 bits per pixel,     8 bits per component,     kCGImageAlphaNoneSkipLast
     32 bits per pixel,     8 bits per component,     kCGImageAlphaPremultipliedFirst
     32 bits per pixel,     8 bits per component,     kCGImageAlphaPremultipliedLast
     64 bits per pixel,    16 bits per component,     kCGImageAlphaPremultipliedLast
     64 bits per pixel,    16 bits per component,     kCGImageAlphaNoneSkipLast
     64 bits per pixel,    16 bits per component,     kCGImageAlphaPremultipliedLast|kCGBitmapFloatComponents
     64 bits per pixel,    16 bits per component,     kCGImageAlphaNoneSkipLast|kCGBitmapFloatComponents
    128 bits per pixel,    32 bits per component,     kCGImageAlphaPremultipliedLast|kCGBitmapFloatComponents
    128 bits per pixel,    32 bits per component,     kCGImageAlphaNoneSkipLast|kCGBitmapFloatComponents
  */
  if (bpc != 8) return cv::Mat();

  size_t const CPP = (cpp == 3) ? 4 : cpp;

  int matType = CV_8UC4;
  switch (CPP)
  {
    case 1: matType = CV_8UC1; break;
    case 3: matType = CV_8UC3; break;
    case 4: matType = CV_8UC4; break;
    default: CV_8UC4;
  }
  cv::Mat cvMat((int)height, (int)width, matType); // 8 bit unsigned per 4 channels

  CGColorSpaceRef colorSpace = (CPP == 1) ? CGColorSpaceCreateDeviceGray() : CGColorSpaceCreateDeviceRGB();
  CGImageAlphaInfo const ALPHAINFO = (cpp == 1) ? kCGImageAlphaNone : ((cpp == 3) ? kCGImageAlphaNoneSkipLast : kCGImageAlphaPremultipliedLast);

  { // lifted from https://gist.github.com/dominiklessel/1716068
    CGContextRef context = CGBitmapContextCreate(cvMat.data, width, height, 8, CPP * width, colorSpace, ALPHAINFO);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    CGContextRelease(context);
  }

  return cvMat;
}


@end
