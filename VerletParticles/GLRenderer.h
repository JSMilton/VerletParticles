//
//  GLRenderer.h
//  GyroSquares
//
//  Created by James Milton on 28/05/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glUtil.h"
#include "Utils.h"

class BillboardShader;
class FeedbackShader;

const int BUFFER_COUNT = 2;

class GLRenderer {
    struct Particle
    {
        glm::vec3 position;
        glm::vec3 previousPosition;
        glm::vec3 acceleration;
    };
    
public:
    void initOpenGL();
    void render(float dt);
    void reshape(int width, int height);
    void destroy();
    
private:
    void createFrameBuffers();
    void resetFramebuffers();
    void freeGLBindings() const;
    
    void initBillboardShader();
    void initFeedbackShader();
    void createParticleBuffers();
    
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
    glm::mat4 mPreviousViewMatrix;
    
    int mViewWidth;
    int mViewHeight;
    
    int mCurrentBuffer;
    
    GLuint mVAO[BUFFER_COUNT];
    GLuint mVBO[BUFFER_COUNT];
    
    BillboardShader *mBillboardShader;
    FeedbackShader *mFeedbackShader;
    
    static NvVertexAttribute ms_attributes[];
};
