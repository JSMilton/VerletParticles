//
//  ObjModelLoader.h
//  SimpleParticleSystem
//
//  Created by James Milton on 11/06/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#ifndef __SimpleParticleSystem__ObjModelLoader__
#define __SimpleParticleSystem__ObjModelLoader__

#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "glUtil.h"

class ObjModelLoader {
public:
    ObjModelLoader(const char* objFileName, bool indexed);
    ~ObjModelLoader();
    
    GLuint mNumVertices;
    
    GLfloat *mPositions;
    GLenum mPositionType;
    GLuint mPositionSize;
    GLsizei mPositionArraySize;
    
    GLubyte *mColors;
    GLenum mColorType;
    GLuint mColorSize;
    GLsizei mColorArraySize;
    
    GLfloat *mNormals;
    GLenum mNormalType;
    GLuint mNormalSize;
    GLsizei mNormalArraySize;
    
    GLfloat *mTextureUV;
    GLenum mTextureType;
    GLuint mTextureSize;
    GLsizei mTexureUVArraySize;
    
    GLuint *mElements;
    GLenum mElementType;
    GLuint mNumElements;
    GLsizei mElementArraySize;
};

#endif /* defined(__SimpleParticleSystem__ObjModelLoader__) */
