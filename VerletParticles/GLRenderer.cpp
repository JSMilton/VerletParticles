//
//  GLRenderer.cpp
//  GyroSquares
//
//  Created by James Milton on 28/05/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "GLRenderer.h"
#include "BillboardShader.h"
#include "FeedbackShader.h"

void GLRenderer::initOpenGL() {
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    mViewWidth = 1200;
    mViewHeight = 800;
    reshape(1200, 800);
    
    createParticleBuffers();
    initBillboardShader();
    initFeedbackShader();
    
    render(0.0);
}

void GLRenderer::initBillboardShader() {
    mBillboardShader = new BillboardShader;
}

void GLRenderer::initFeedbackShader() {
    mFeedbackShader = new FeedbackShader;
    const GLchar* FeedbackVaryings[2] =
    {
        "vPosition",
        "vVelocity",
    };
    
    glTransformFeedbackVaryings(mFeedbackShader->getProgram(),countof(FeedbackVaryings),
                                FeedbackVaryings,GL_INTERLEAVED_ATTRIBS);
    mFeedbackShader->linkProgram();
}

void GLRenderer::createParticleBuffers() {
    Particle particles[MAX_PARTICLES];
    float width, height;
    width = sqrt((float)MAX_PARTICLES);
    height = width;
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int index = i * width + j;
            particles[index].position.x = i / width * 2 - 1.0;
            particles[index].position.y = j / height * 2 - 1.0;
            particles[index].position.z = 1;
            particles[index].velocity = glm::vec3(0,0,0);
        }
    }
    
    glGenBuffers(BUFFER_COUNT, mVBO);
    glGenVertexArrays(BUFFER_COUNT, mVAO);
    for (int i = 0; i < BUFFER_COUNT; i++){
        glBindVertexArray(mVAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Particle), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Particle), (void*)(sizeof(GLfloat)*3));
    }
    
    glBindVertexArray(0);
}

void GLRenderer::drawParticles() {
    
}

void GLRenderer::render(float dt) {
    const float FPS_CLAMP = 1.0f / 15.0f;
    if (dt > FPS_CLAMP)
        dt = FPS_CLAMP;
    
    freeGLBindings();
    
    glm::mat4 mvp = mProjectionMatrix * mViewMatrix;
    glm::vec4 right = mViewMatrix[0];
    glm::vec4 up = mViewMatrix[1];
    
    mFeedbackShader->enable();
    glUniform1f(mFeedbackShader->mDeltaTimeHandle, dt);
    glUniform3f(mFeedbackShader->mMousePositionHandle, mMousePosition.x, mMousePosition.y, mMousePosition.z);
    glUniform3f(mFeedbackShader->mMouseAccelerationHandle, mMouseAcceleration.x, mMouseAcceleration.y * -1, mMouseAcceleration.z);
    glUniformMatrix4fv(mFeedbackShader->mModelViewProjectionMatrixHandle, 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(mVAO[(mCurrentBuffer+1)%BUFFER_COUNT]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVBO[mCurrentBuffer]);
    glEnable(GL_RASTERIZER_DISCARD);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    mBillboardShader->enable();
    glUniformMatrix4fv(mBillboardShader->mModelViewProjectionHandle, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(mBillboardShader->mRightHandle, right.x, right.y, right.z);
    glUniform3f(mBillboardShader->mUpHandle, up.x, up.y, up.z);
    glUniform1f(mBillboardShader->mBillboardSizeHandle, BILLBOARD_SIZE);
    glBindVertexArray(mVAO[mCurrentBuffer]);
    glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
    
    mCurrentBuffer = (mCurrentBuffer + 1) % BUFFER_COUNT;
}

void GLRenderer::reshape(int width, int height) {
    glViewport(0, 0, width, height);
    mViewWidth = width;
    mViewHeight = height;
    mProjectionMatrix = glm::perspective(45.0f, (float)width/(float)height, 0.1f, 100.0f);
    mViewMatrix = glm::lookAt(glm::vec3(0,0,7), glm::vec3(0,0,0), glm::vec3(0,1,0));
    
    createFrameBuffers();
}

void GLRenderer::freeGLBindings(void) const
{
    glBindFramebuffer(GL_FRAMEBUFFER,     0);
    glBindRenderbuffer(GL_RENDERBUFFER,   0);
    glBindBuffer(GL_ARRAY_BUFFER,         0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D,          0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,    0);
}

void GLRenderer::getMousePosition(float x, float y, float z) {
    mMousePosition = glm::vec3(x,y,z);
}

void GLRenderer::getMouseAcceleration(float x, float y, float z) {
    mMouseAcceleration = glm::vec3(x,y,z);
}

void GLRenderer::resetFramebuffers() {
}

void GLRenderer::createFrameBuffers() {
    resetFramebuffers();
}

void GLRenderer::destroy() {
}