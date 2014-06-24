//
//  BillboardShader.cpp
//  NoiseBasedParticles
//
//  Created by James Milton on 18/06/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "BillboardShader.h"

BillboardShader::BillboardShader() : BaseShader("billboard", "billboard", "billboard") {
    mModelViewProjectionHandle = getUniformLocation("u_MVP");
    mRightHandle = getUniformLocation("u_Right");
    mUpHandle = getUniformLocation("u_Up");
    mBillboardSizeHandle = getUniformLocation("u_BillboardSize");
}

