//
//  FeedbackShader.cpp
//  NoiseBasedParticles
//
//  Created by James Milton on 18/06/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "FeedbackShader.h"

FeedbackShader::FeedbackShader() : BaseShader("feedback", NULL, "feedback") {
    mTimeHandle = getUniformLocation("u_Time");
    mDeltaTimeHandle = getUniformLocation("u_DeltaTime");
    mCTimeHandle = getUniformLocation("u_cTime");
    mRandomTextureHandle = getUniformLocation("u_RandomTexture");
    mFBMTextureHandle = getUniformLocation("u_FBMTexture");
    mParticleLifetimeHandle = getUniformLocation("u_ParticleLifetime");
    
    printf("%i\n", mTimeHandle);
    printf("%i\n", mDeltaTimeHandle);
    printf("%i\n", mCTimeHandle);
    printf("%i\n", mRandomTextureHandle);
    printf("%i\n", mFBMTextureHandle);
}
