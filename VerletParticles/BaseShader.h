//
//  BaseShader.h
//  GyroSquares
//
//  Created by James Milton on 28/05/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#ifndef BASE_SHADER_H
#define BASE_SHADER_H

#include "glUtil.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

class BaseShader {
public:
    BaseShader(const char* vShader, const char* fShader, const char* gShader);
    void linkProgram();
    GLint getUniformLocation(const char* uniformName);
    GLint getAttribLocation(const char* attribName);
    GLuint getProgram();
    void enable();
    void disable();
    
protected:
    GLuint mProgram;
};

#endif // BASE_SHADER_H
