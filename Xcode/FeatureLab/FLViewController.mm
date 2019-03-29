//
//  ViewController.m
//  FeatureLab
//
//  Created by Kai Oezer on 6/14/18.
//  Copyright © 2018 Kai Oezer. All rights reserved.
//

#import "FLViewController.h"
#import "FLInformationOverlayView.h"
#import "FLImageView.h"
#import "FLImageUtilities.h"

#include <vector>
#include <iostream>

#include "preprocessing.hpp"
#include "line_detector.hpp"
#include "other_detectors.hpp"

using namespace std;

// User defaults keys
NSString* const kLastUsedImageFileLocationKey = @"LastUsedImageFileLocation";
NSString* const kIntensityReductionValueKey = @"IntensityReductionValue";
NSString* const kUsedChannelKey = @"UsedChannel";
NSString* const kSelectedDetectorKey = @"Detector";
NSString* const kAssumeRAWImageKey = @"RAWImage";
NSString* const kNormalizeIntensityKey = @"NormalizeIntensity";


typedef NS_ENUM(NSInteger, FLKeyPointDetector)
{
  FLKeyPointDetectorGFTT          = 0,
  FLKeyPointDetectorSIFT          = 1,
  FLKeyPointDetectorSURF          = 2,
  FLKeyPointDetectorFAST          = 3,
  FLKeyPointDetectorAGAST         = 4,
  FLKeyPointDetectorBRISK         = 5,
  FLKeyPointDetectorORB           = 6,
  FLKeyPointDetectorHarrisLaplace = 7,
  FLKeyPointDetectorStar          = 8,
  FLKeyPointDetectorKAZE          = 9,
  FLKeyPointDetectorAKAZE         = 10,
};


//MARK: -

@interface FLViewController () <FLImageViewDelegate>
@end


@implementation FLViewController
{
  IBOutlet FLImageView* imageView;
  IBOutlet NSPopUpButton* channelSelector;
  IBOutlet NSSlider* intensityReductionSlider;
  IBOutlet NSButton* normalizeIntensityButton;
  IBOutlet NSButton* rawImageButton;
  IBOutlet NSPopUpButton* keyPointDetectorSelector;

@private
  FLInformationOverlayView* _overlayView;
  NSImage* _image;
  CGImageRef _cachedCGImage;
  BOOL _isRAWImage;
  BOOL _isSingleChannelImage;
  robofish::Channel _usedChannel;
  BOOL _normalizeIntensity;
  float _intensityReduction; // when < 0, intensity reduction is off
  FLKeyPointDetector _detector;
}

- (void)dealloc
{
  if (_cachedCGImage != nil)
  {
    CGImageRelease(_cachedCGImage);
  }
}

- (void) awakeFromNib
{
  [super awakeFromNib];
  NSUserDefaults* def = [NSUserDefaults standardUserDefaults];
  _detector = ([def objectForKey:kSelectedDetectorKey] != nil) ? (FLKeyPointDetector)[def integerForKey:kSelectedDetectorKey] : FLKeyPointDetectorGFTT;
  _usedChannel = ([def objectForKey:kUsedChannelKey] != nil) ? robofish::Channel([def integerForKey:kUsedChannelKey]) : robofish::Channel::gray;
  _normalizeIntensity = ([def objectForKey:kNormalizeIntensityKey] != nil) ? [def boolForKey:kNormalizeIntensityKey] : NO;
  _intensityReduction = ([def objectForKey:kIntensityReductionValueKey] != nil) ? [def floatForKey:kIntensityReductionValueKey] : -1;
  _isRAWImage = ([def objectForKey:kAssumeRAWImageKey] != nil) ? [def boolForKey:kAssumeRAWImageKey] : NO;
  [self _updateControlsFromModel];
}

- (void) viewDidLoad
{
  [super viewDidLoad];
  [imageView setDelegate:self];
  [self _setUpOverlayView];
  [self _restoreLastProcessedImage];
  [self _updateModelForImage];
  [self _updateControlsFromModel];
  [self _processImage];
}

- (void)setRepresentedObject:(id)representedObject
{
  [super setRepresentedObject:representedObject];

  // Update the view, if already loaded.
}

- (void) openDocument:(NSMenuItem*)menuItem
{
  NSOpenPanel* openPanel = [NSOpenPanel openPanel];
  openPanel.allowedFileTypes = @[@"bmp", @"jpeg", @"jpg", @"png"];
  openPanel.allowsMultipleSelection = NO;
  openPanel.worksWhenModal = YES;
  [openPanel beginSheetModalForWindow:[self.view window] completionHandler:^(NSModalResponse returnCode) {
    if (returnCode == NSModalResponseOK)
    {
      NSArray<NSURL*>* locations = [openPanel URLs];
      if ([locations count] > 0)
      {
        NSURL* firstURL = [locations firstObject];
        [self _loadImageFromFileLocation:firstURL];
      }
    }
  }];
}

- (IBAction) changeDetector:(id)sender
{
  _detector = (FLKeyPointDetector)[(NSPopUpButton*)sender selectedTag];
  [[NSUserDefaults standardUserDefaults] setInteger:_detector forKey:kSelectedDetectorKey];
  [self _processImage];
}

- (IBAction) assumeRaw:(id)sender
{
  _isRAWImage = (rawImageButton.state == NSControlStateValueOn);
  [[NSUserDefaults standardUserDefaults] setBool:_isRAWImage forKey:kAssumeRAWImageKey];
  [self _updateControlsFromModel];
}

- (IBAction) changeIntensityReductionValue:(id)sender
{
  _intensityReduction = intensityReductionSlider.floatValue;
  [[NSUserDefaults standardUserDefaults] setFloat:_intensityReduction forKey:kIntensityReductionValueKey];
  [self _processImage];
}

- (IBAction) normalizeIntensity:(id)sender
{
  _normalizeIntensity = normalizeIntensityButton.state == NSControlStateValueOn;
  [[NSUserDefaults standardUserDefaults] setBool:_normalizeIntensity forKey:kNormalizeIntensityKey];
  intensityReductionSlider.enabled = _normalizeIntensity;
  intensityReductionSlider.floatValue = _intensityReduction;
  [self _processImage];
}

- (IBAction) selectChannel:(id)sender
{
  _usedChannel = robofish::Channel(channelSelector.selectedTag);
  [[NSUserDefaults standardUserDefaults] setInteger:(int)_usedChannel forKey:kUsedChannelKey];
  [self _processImage];
}

- (BOOL) validateMenuItem:(NSMenuItem*)menuItem
{
  if (menuItem.menu == channelSelector.menu)
  {
    return (menuItem.tag == 0) || !_isSingleChannelImage || _isRAWImage;
  }
  else if (menuItem.menu == keyPointDetectorSelector.menu)
  {
    return YES;
  }
  return [super validateMenuItem:menuItem];
}

//MARK: FLImageViewDelegate

- (BOOL) handleDroppedFileLocation:(NSURL*)fileLocation
{
  return [self _loadImageFromFileLocation:fileLocation];
}

//MARK: Private

- (BOOL) _loadImageFromFileLocation:(NSURL*)location
{
  _image = [[NSImage alloc] initWithContentsOfURL:location];
  if (_image != nil)
  {
    dispatch_async(dispatch_get_main_queue(), ^{
      [self _updateModelForImage];
      [self _updateControlsFromModel];
      [self _processImage];
      [self _saveLastUsedImageLocation:location];
    });
    return YES;
  }
  return NO;
}

- (void) _saveLastUsedImageLocation:(NSURL*)imageFileLocation
{
  [[NSUserDefaults standardUserDefaults] setURL:imageFileLocation forKey:kLastUsedImageFileLocationKey];
}

- (NSURL*) _restoreLastUsedImageLocation
{
  return [[NSUserDefaults standardUserDefaults] URLForKey:kLastUsedImageFileLocationKey];
}

- (void) _restoreLastProcessedImage
{
  NSURL* imageLocation = [self _restoreLastUsedImageLocation];
  if (imageLocation != nil)
  {
    _image = [[NSImage alloc] initWithContentsOfURL:imageLocation];
  }
}

- (void) _updateModelForImage
{
  CGImageRelease(_cachedCGImage);
  _cachedCGImage = nil;
  if (_image == nil)
  {
    [_overlayView showMessage:@"Drag image file here"];
    _usedChannel = robofish::Channel::gray;
    _isRAWImage = NO;
    _isSingleChannelImage = NO;
    _normalizeIntensity = NO;
    _intensityReduction = 1.0;
  }
  else
  {
    [_overlayView clearMessage];
    imageView.image = _image;
    CGColorSpaceRef cp = CGImageGetColorSpace([self _cgImage]);
    long const numComponents = CGColorSpaceGetNumberOfComponents(cp);
    _isSingleChannelImage = (cp != nil) ? (numComponents == 1) : NO;

    if (_isSingleChannelImage && (_usedChannel > robofish::Channel::gray) && !_isRAWImage)
    {
      _usedChannel = robofish::Channel::gray;
    }
  }
}

- (void) _updateControlsFromModel
{
  rawImageButton.enabled = _isRAWImage || _isSingleChannelImage;
  rawImageButton.state = _isRAWImage ? NSControlStateValueOn : NSControlStateValueOff;
  normalizeIntensityButton.state = _normalizeIntensity ? NSControlStateValueOn : NSControlStateValueOff;
  intensityReductionSlider.enabled = _normalizeIntensity;
  intensityReductionSlider.floatValue = _intensityReduction;
  [channelSelector selectItemWithTag:(NSInteger)_usedChannel];
  [keyPointDetectorSelector selectItemWithTag:_detector];
}

- (CGImageRef) _cgImage
{
  if (_cachedCGImage == nil && _image != nil)
  {
    _cachedCGImage = [FLImageUtilities createCGImageFromNSImage:_image];
  }
  return _cachedCGImage;
}

- (void) _processImage
{
  [_overlayView clearPoints];

  CGImageRef img = [self _cgImage];
  if (img != nil)
  {
    cv::Mat mat = [FLImageUtilities matFromCgImage:img];

    if (_isRAWImage && _isSingleChannelImage)
    {
      mat = robofish::convertRAWToRGB(mat);
    }

    mat = robofish::extractChannel(mat, _usedChannel);

    if (_normalizeIntensity)
    {
      robofish::normalizeIntensity(mat);

      if (_intensityReduction <= 1.0)
      {
        robofish::reduceIntensity(mat, _intensityReduction);
      }
    }

    std::vector<cv::KeyPoint> keyPoints;
    robofish::findKeypoints((uint8_t)_detector, mat, keyPoints);
    imageView.image = [FLImageUtilities imageFromMat:mat];

  #if 0
    cout << "Features:" << endl;
    for (auto keyPoint : keyPoints)
    {
      cout << "  " << keyPoint.pt.x << "," << keyPoint.pt.y << endl;
    }
  #endif
    [_overlayView setKeyPoints:std::move(keyPoints) imageDimensions:CGSizeMake(mat.cols, mat.rows)];

  }
}

- (void) _setUpOverlayView
{
  _overlayView = [[FLInformationOverlayView alloc] initWithFrame:CGRectZero];
  [imageView addSubview:_overlayView];
  _overlayView.translatesAutoresizingMaskIntoConstraints = false;
  [_overlayView.topAnchor constraintEqualToAnchor:imageView.topAnchor].active = YES;
  [_overlayView.bottomAnchor constraintEqualToAnchor:imageView.bottomAnchor].active = YES;
  [_overlayView.leftAnchor constraintEqualToAnchor:imageView.leftAnchor].active = YES;
  [_overlayView.rightAnchor constraintEqualToAnchor:imageView.rightAnchor].active = YES;
  _overlayView.frame = imageView.bounds;
}

@end
