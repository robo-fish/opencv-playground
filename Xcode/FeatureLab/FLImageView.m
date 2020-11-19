//
//  FLImageView.m
//  FeatureLab
//
//  Created by Kai Oezer on 6/17/18.
//  Copyright © 2018 Kai Oezer. All rights reserved.
//

#import "FLImageView.h"


@implementation FLImageView
{
@private
	BOOL _showHighlightFrame;
}

- (instancetype) initWithFrame:(NSRect)frameRect
{
	self = [super initWithFrame:frameRect];
	[self _commonInit];
	return self;
}

- (instancetype) initWithCoder:(NSCoder*)decoder
{
	self = [super initWithCoder:decoder];
	[self _commonInit];
	return self;
}

- (void) _commonInit
{
	[self registerForDraggedTypes:@[NSPasteboardTypeFileURL]];
}

- (void) setImage:(NSImage*)image
{
	_image = image;
	[self setNeedsDisplay:YES];
}

- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)info
{
	if ([info.draggingPasteboard canReadObjectForClasses:@[[NSURL class]] options:nil])
	{
		_showHighlightFrame = YES;
		[self setNeedsDisplay:YES];
		return NSDragOperationGeneric;
	}
	return NSDragOperationNone;
}

- (void) draggingExited:(id<NSDraggingInfo>)sender
{
	if (_showHighlightFrame)
	{
		_showHighlightFrame = NO;
		[self setNeedsDisplay:YES];
	}
}

- (BOOL) performDragOperation:(id <NSDraggingInfo>)info;
{
	if ([info.draggingPasteboard canReadObjectForClasses:@[[NSURL class]] options:nil])
	{
		NSArray* urls = [info.draggingPasteboard readObjectsForClasses:@[[NSURL class]] options:nil];
		if ([urls count] > 0)
		{
			NSURL* firstFileLocation = (NSURL*)[urls firstObject];
			BOOL const fileAccepted = [self.delegate handleDroppedFileLocation:firstFileLocation];
			_showHighlightFrame = _showHighlightFrame && !fileAccepted;
			[self setNeedsDisplay:YES];
			return fileAccepted;
		}
	}
	return NO;
}

- (void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];
	if (_image != nil)
	{
		[_image drawInRect:self.bounds];
	}
	if (_showHighlightFrame)
	{
		CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
		CGContextSaveGState(context);
		CGFloat highlightColor[] = { 0.2, 0.2, 1.0, 1.0 };
		CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
		CGContextSetStrokeColorSpace(context, colorspace);
		CGContextSetStrokeColor(context, highlightColor);
		CGContextSetLineWidth(context, 4.0);
		CGContextBeginPath(context);
		CGContextAddRect(context, CGRectInset(self.bounds, 2, 2));
		CGContextStrokePath(context);
		CGContextRestoreGState(context);
	}
}

@end
