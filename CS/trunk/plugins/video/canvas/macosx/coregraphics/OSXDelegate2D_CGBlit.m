//
//  OSXDelegate2D_OpenGL.m
//
//
//  Created by mreda on Wed Oct 31 2001.
//  Copyright (c) 2001 Matt Reda. All rights reserved.
//

#import "video/canvas/macosx/common/OSXDelegate2D.h"


// Initialized on first use, destroyed at program termination
static CGColorSpaceRef colorSpace = NULL;        
    


////////// OSXDelegate (CGBlit) category


@interface OSXDelegate2D (CGBlit)

// Blit buffer to the object's window
- (BOOL) blitToWindow:(unsigned char *) buffer width:(int) width height:(int) height depth:(int) depth;

@end



@implementation OSXDelegate2D (CGBlit)


- (BOOL) blitToWindow:(unsigned char *) buffer width:(int) width height:(int) height depth:(int) depth
{
    NSView *contentView = [window contentView];

    if (window == nil)
        return NO;

    if (colorSpace == NULL)
        colorSpace = CGColorSpaceCreateDeviceRGB();

    if ([contentView lockFocusIfCanDraw] == YES)
    {
        // We only need new versions of these when the backing buffer changes
        // Using static variables will be a problem if the user creates more than
        // one canvas at a time
        static CGDataProviderRef prov = NULL;
        static CGImageRef image = NULL;
        static CGRect rect;

        if ((prov == NULL) || 
            (width != rect.size.width) || (height != rect.size.height))
        {
            size_t bytesPerPixel = depth / 8;
            size_t bitsPerComponent = (depth == 32) ? 8 : 5;
            size_t bytesPerRow = bytesPerPixel * width;
            size_t bufferSize = height * bytesPerRow;

            if (prov != NULL)
            {
                CGDataProviderRelease(prov);
                CGImageRelease(image);            
            }

            prov = CGDataProviderCreateWithData(NULL, buffer, bufferSize, NULL);
            image = CGImageCreate(width, height, bitsPerComponent, depth, 
                                bytesPerRow, colorSpace, kCGImageAlphaNoneSkipFirst, 
                                prov, NULL, NO, kCGRenderingIntentDefault);
            rect = CGRectMake(0, 0, width, height);
        }
        
        CGContextDrawImage([[NSGraphicsContext currentContext] graphicsPort], 
                            rect, image);

        [window flushWindow];
        [contentView unlockFocus];

        return YES;
    }

    return NO;
}


@end


// C API to OSXDelegate2D class (CGBlit category)
#define DEL2D_FUNC(ret, func) __private_extern__ inline ret OSXDelegate2D_##func

typedef void *OSXDelegate2DHandle;


// C API to driver delegate class - wrappers around methods
DEL2D_FUNC(bool, blitToWindow)(OSXDelegate2DHandle delegate, unsigned char *buffer, int width, int height, int depth)
{
    return [(OSXDelegate2D *) delegate blitToWindow:buffer width:width height:height depth:depth];
}

#undef DEL2D_FUNC
