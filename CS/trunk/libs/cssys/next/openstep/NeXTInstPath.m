/*
   NeXTInstPath.m

    Created by Matt Reda <mreda@mac.com>
    Copyright (c) 2002 Matt Reda. All rights reserved.
 */


#import <Foundation/Foundation.h>

int NeXTGetInstallPath(char *oInstallPath, size_t iBufferSize, char pathSep)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSString* s = [defs stringForKey:@"CrystalSpaceRootIgnore"];
    int result = 0;
        
    oInstallPath[0] = '\0';
    if (s == 0 || [s isEqualToString:@""] ||
        (![s hasPrefix:@"Y"] && ![s hasPrefix:@"y"] &&	// Yes
        ![s hasPrefix:@"T"] && ![s hasPrefix:@"t"] &&	// True
        ![s hasPrefix:@"O"] && ![s hasPrefix:@"o"] &&	// On
        ![s hasPrefix:@"1"]))				// 1
    {
        s = [defs stringForKey:@"CrystalSpaceRoot"];
        if (s != 0 && ![s isEqualToString:@""])
        {
            NSMutableString *path = [s mutableCopy];
	    int const n = [path length];
	    // >=2 to avoid stripping "/" from path if path is root directory.
            if (n >= 2 && [path characterAtIndex:n - 1] != pathSep)
                [path appendFormat:@"%c", pathSep];
                
            [path getFileSystemRepresentation:oInstallPath maxLength:iBufferSize];
            [path release];
            result = 1;
        }
    }

    [pool release];
    return result;
}
