//
//  GLRenderer.cpp
//  GyroSquares
//
//  Created by James Milton on 28/05/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "GLRenderer.h"

NvVertexAttribute GLRenderer::ms_attributes[] =
{
    {"a_Position", GL_FLOAT, 3, offsetof(Particle,position),sizeof(Particle),false},
    {"a_PreviousPosition", GL_FLOAT, 3, offsetof(Particle,previousPosition),sizeof(Particle),false},
    {"a_Acceleration", GL_FLOAT, 3, offsetof(Particle,acceleration),sizeof(Particle),false},
    
    {NULL,0,0,0,0,false}
};

void GLRenderer::initOpenGL() {
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    mViewWidth = 1200;
    mViewHeight = 800;
    reshape(1200, 800);
    
    createParticleBuffers();
    
    render(0.0);
}

void GLRenderer::initBillboardShader() {
    
}

void GLRenderer::initFeedbackShader() {
    
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
            particles[index].previousPosition = particles[index].position;
            particles[index].acceleration = glm::vec3(0,0,0);
        }
    }
    
    glGenBuffers(BUFFER_COUNT, mVBO);
    glGenVertexArrays(BUFFER_COUNT, mVAO);
    for (int i = 0; i < BUFFER_COUNT; i++){
        glBindVertexArray(mVAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STATIC_DRAW);
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
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void GLRenderer::resetFramebuffers() {
}

void GLRenderer::createFrameBuffers() {
    resetFramebuffers();
}

void GLRenderer::destroy() {
}