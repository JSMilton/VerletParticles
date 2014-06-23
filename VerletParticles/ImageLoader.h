//
//  ImageLoader.h
//  SimpleParticleSystem
//
//  Created by James Milton on 05/06/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#ifndef __SimpleParticleSystem__ImageLoader__
#define __SimpleParticleSystem__ImageLoader__

#include <iostream>
#include "glUtil.h"

class ImageLoader {
public:
    ImageLoader(const char* imageName);
    ~ImageLoader();
    
    GLubyte *mImageData;
    size_t mWidth;
    size_t mHeight;
};

#endif /* defined(__SimpleParticleSystem__ImageLoader__) */
