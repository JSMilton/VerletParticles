//
//  BillboardShader.h
//  NoiseBasedParticles
//
//  Created by James Milton on 18/06/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "BaseShader.h"

class BillboardShader : public BaseShader {
public:
    BillboardShader();

    GLint mModelViewProjectionHandle;
    GLint mRightHandle;
    GLint mUpHandle;
    GLint mBillboardSizeHandle;
};