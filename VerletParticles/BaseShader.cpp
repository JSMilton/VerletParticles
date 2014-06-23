//
//  BaseShader.cpp
//  GyroSquares
//
//  Created by James Milton on 28/05/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "BaseShader.h"
#include <CoreFoundation/CoreFoundation.h>

GLchar* getShaderString(const char* shaderName, const char* shaderType) {
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    
    CFStringRef shaderNameRef = CFStringCreateWithCStringNoCopy(NULL, shaderName,
                                                   kCFStringEncodingASCII, NULL);
    
    CFStringRef shaderTypeRef = CFStringCreateWithCStringNoCopy(NULL, shaderType,
                                                   kCFStringEncodingASCII, NULL);
    
    // Get a reference to the file's URL
    CFURLRef shaderURL = CFBundleCopyResourceURL(mainBundle, shaderNameRef, shaderTypeRef, NULL);
    
    // Convert the URL reference into a string reference
    CFStringRef shaderPathRef = CFURLCopyFileSystemPath(shaderURL, kCFURLPOSIXPathStyle);
    
    // Get the system encoding method
    CFStringEncoding encodingMethod = CFStringGetSystemEncoding();
    
    // Convert the string reference into a C string
    const char *shaderPath = CFStringGetCStringPtr(shaderPathRef, encodingMethod);
    
    fprintf(stderr, "Shader file loaded from path: %s\n", shaderPath);
    
    GLchar* sourceString = NULL;
    float  glLanguageVersion;
	sscanf((char *)glGetString(GL_SHADING_LANGUAGE_VERSION), "%f", &glLanguageVersion);
    GLuint version = 100 * glLanguageVersion;
	
	// Get the size of the version preprocessor string info so we know
	//  how much memory to allocate for our sourceString
	const GLsizei versionStringSize = sizeof("#version 123\n");
    
    FILE* curFile = fopen(shaderPath, "r");
	
	// Get the size of the source
	fseek(curFile, 0, SEEK_END);
	GLsizei fileSize = ftell (curFile);
    //fileSize += 1; // for null terminator
	
	// Alloc memory for the string
	GLchar *string = (GLchar *)malloc(fileSize+1);
	
	// Read entire file into the string from beginning of the file
	fseek(curFile, 0, SEEK_SET);
	fread(string, 1, fileSize, curFile);
	
	fclose(curFile);
	
	// Insert null terminator
	string[fileSize] = 0;

    
    sourceString = (GLchar *)malloc(fileSize+1 + versionStringSize);
    
   // printf("VERSION IS: %d\n", version);
	
	// Prepend our vertex shader source string with the supported GLSL version so
	//  the shader will work on ES, Legacy, and OpenGL 3.2 Core Profile contexts
	sprintf(sourceString, "#version %d\n%s", version, string);
    
    return sourceString;
}

BaseShader::BaseShader(const char* vShader, const char* fShader, const char* gShader) {
	GLint logLength, status;
    GLchar* sourceString = NULL;
    
    GetGLError();
	
	// Create a program object
	mProgram = glCreateProgram();
    
    GetGLError();
    
    if (vShader){
        // Allocate memory for the source string including the version preprocessor information
        sourceString = getShaderString(vShader, "vsh");
        
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, (const GLchar **)&(sourceString), NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength > 0)
        {
            GLchar *log = (GLchar*) malloc(logLength);
            glGetShaderInfoLog(vertexShader, logLength, &logLength, log);
            printf("Vtx shader compiled log: %s\n", log);
            free(log);
        }
        
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            printf("Failed to compile vtx shader:\n%s\n", sourceString);
        }
        
        free(sourceString);
        sourceString = NULL;
        
        // Attach the vertex shader to our program
        glAttachShader(mProgram, vertexShader);
        
        // Delete the vertex shader since it is now attached
        // to the program, which will retain a reference to it
        glDeleteShader(vertexShader);
    }
    
    if (gShader){
        sourceString = getShaderString(gShader, "gsh");
        
        GLuint fragShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(fragShader, 1, (const GLchar **)&(sourceString), NULL);
        glCompileShader(fragShader);
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar*)malloc(logLength);
            glGetShaderInfoLog(fragShader, logLength, &logLength, log);
            printf("Frag Shader compile log:\n%s\n", log);
            free(log);
        }
        
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            printf("Failed to compile frag shader:\n%s\n", sourceString);
        }
        
        free(sourceString);
        sourceString = NULL;
        
        // Attach the fragment shader to our program
        glAttachShader(mProgram, fragShader);
        
        // Delete the fragment shader since it is now attached
        // to the program, which will retain a reference to it
        glDeleteShader(fragShader);
    }
    
    if (fShader){
        // Allocate memory for the source string including the version preprocessor	 information
        sourceString = getShaderString(fShader, "fsh");
        
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, (const GLchar **)&(sourceString), NULL);
        glCompileShader(fragShader);
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar*)malloc(logLength);
            glGetShaderInfoLog(fragShader, logLength, &logLength, log);
            printf("Frag Shader compile log:\n%s\n", log);
            free(log);
        }
        
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            printf("Failed to compile frag shader:\n%s\n", sourceString);
        }
        
        free(sourceString);
        sourceString = NULL;
        
        // Attach the fragment shader to our program
        glAttachShader(mProgram, fragShader);
        
        // Delete the fragment shader since it is now attached
        // to the program, which will retain a reference to it
        glDeleteShader(fragShader);
    }
    
    linkProgram();
}

void BaseShader::linkProgram() {
    GLint logLength, status;
    glLinkProgram(mProgram);
	glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(mProgram, logLength, &logLength, log);
		printf("Program link log:\n%s\n", log);
		free(log);
	}
	
	glGetProgramiv(mProgram, GL_LINK_STATUS, &status);
	if (status == 0)
	{
		printf("Failed to link program\n");
	}
	
	glValidateProgram(mProgram);
	glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(mProgram, logLength, &logLength, log);
		printf("Program validate log:\n%s\n", log);
		free(log);
	}
	
	glGetProgramiv(mProgram, GL_VALIDATE_STATUS, &status);
	if (status == 0)
	{
		printf("Failed to validate program\n");
	}
    
    printf("prog is: %i\n", mProgram);
	
	GetGLError();
}

GLint BaseShader::getUniformLocation(const char *uniformName) {
    return glGetUniformLocation(mProgram, uniformName);
}

GLint BaseShader::getAttribLocation(const char *attribName) {
    return glGetAttribLocation(mProgram, attribName);
}

GLuint BaseShader::getProgram() {
    return mProgram;
}

void BaseShader::enable() {
    glUseProgram(mProgram);
}

void BaseShader::disable() {
    glUseProgram(0);
}