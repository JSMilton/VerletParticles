//
//  ImageLoader.cpp
//  SimpleParticleSystem
//
//  Created by James Milton on 05/06/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "ImageLoader.h"
#include <CoreFoundation/CoreFoundation.h>
#include <QuartzCore/QuartzCore.h>

ImageLoader::ImageLoader(const char* imageName) {
    CGImageRef image;
    CGDataProviderRef provider;
    CFStringRef name;
    CFURLRef url;
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    
    // Get the URL to the bundle resource.
    name = CFStringCreateWithCString (NULL, imageName, kCFStringEncodingUTF8);
    url = CFBundleCopyResourceURL(mainBundle, name, CFSTR("png"), NULL);
    CFRelease(name);
    
    // Create the data provider object
    provider = CGDataProviderCreateWithURL (url);
    CFRelease (url);
    
    // Create the image object from that provider.
    image = CGImageCreateWithPNGDataProvider(provider, NULL, true, kCGRenderingIntentDefault);
    CGDataProviderRelease (provider);
    
    mWidth = CGImageGetWidth(image);
    mHeight = CGImageGetHeight(image);
    //mImageData = (GLubyte *)malloc(mWidth * mHeight * 4);
    
    CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(image));
    mImageData = (GLubyte *)CFDataGetBytePtr(data);
    
    CGContextRef context = CGBitmapContextCreate(mImageData, mWidth, mHeight, 8, mWidth * 4, CGImageGetColorSpace(image), kCGImageAlphaNoneSkipFirst);
	CGContextSetBlendMode(context, kCGBlendModeCopy);
	CGContextDrawImage(context, CGRectMake(0.0, 0.0, mWidth, mHeight), image);
	CGContextRelease(context);
    CGImageRelease(image);
    CFRelease(data);
}

ImageLoader::~ImageLoader() {
   // free(mImageData);
}