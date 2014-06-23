//
//  ObjModelLoader.cpp
//  SimpleParticleSystem
//
//  Created by James Milton on 11/06/2014.
//  Copyright (c) 2014 James Milton. All rights reserved.
//

#include "ObjModelLoader.h"
#include <CoreFoundation/CoreFoundation.h>

typedef struct {
    float x;
    float y;
    float z;
} vector;

typedef struct {
    int x;
    int y;
    int z;
} iVector;

typedef struct {
    iVector xVec;
    iVector yVec;
    iVector zVec;
} element;


char* getSubstringUpToDelimiter(int startIndex, const char *substr, char delimiter) {
    int j = startIndex;
    // we have found a vertex. lets get it!
    int end = 0;
    while (!end) {
        if (substr[j] == delimiter){
            end = 1;
        } else {
            j++;
        }
    }
    
    int length = j-startIndex;
    
    char *newStr = (char*)malloc(length+1);
    strncpy(newStr, substr+startIndex, length);
    newStr[length] = '\0';
    return newStr;
}

vector getVectorFromSubstring(char *substr, const char* delimiter){
    vector vVector;
    
    const char *x = strtok(substr, delimiter);
    if (x == NULL)x = "0";
    const char *y = strtok(NULL, delimiter);
    if (y == NULL)y = "0";
    const char *z = strtok(NULL, delimiter);
    if (z == NULL)z = "0";

    vVector.x = strtof(x, NULL);
    vVector.y = strtof(y, NULL);
    vVector.z = strtof(z, NULL);
    return vVector;
}

iVector getIVectorFromSubstring(char *substr, const char* delimiter){
    iVector vVector;
    
    const char *x = strtok(substr, delimiter);
    if (x == NULL)x = "0";
    const char *y = strtok(NULL, delimiter);
    if (y == NULL)y = "0";
    const char *z = strtok(NULL, delimiter);
    if (z == NULL)z = "0";
    
    printf("%s %s %s\n", x,y,z);
    
    vVector.x = (int)strtol(x, NULL, 10);
    vVector.y = (int)strtol(y, NULL, 10);
    vVector.z = (int)strtol(z, NULL, 10);
    
    return vVector;
}

ObjModelLoader::ObjModelLoader(const char* objFileName, bool indexed) {
    
    CFStringRef name;
    CFMutableStringRef filePath;
    CFURLRef url;
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    
    // Get the URL to the bundle resource.
    name = CFStringCreateWithCString (NULL, objFileName, kCFStringEncodingUTF8);
    url = CFBundleCopyResourceURL(mainBundle, name, CFSTR("obj"), NULL);
    filePath = CFStringCreateMutableCopy(NULL, INT16_MAX, CFURLGetString(url));
    CFStringReplace(filePath, CFRangeMake(0, 7), CFSTR(""));
    char str[INT16_MAX];
    CFStringGetCString(filePath, str, INT16_MAX, kCFStringEncodingASCII);
    
    char *buffer = 0;
    
    FILE *curFile = fopen(str, "r");
    
    if (curFile){
        fseek(curFile, 0, SEEK_END);
        size_t fileSize = ftell(curFile);
        buffer = (char*)malloc(fileSize);
        fseek(curFile, 0, SEEK_SET);
        fread(buffer, 1, fileSize, curFile);
        
        int vertexCount = 0;
        int elementCount = 0;
        int normalCount = 0;
        int textureCount = 0;
        
        int hasPositions = 0;
        int hasNormals = 0;
        int hasTextures = 0;
        
        for (int i = 0; i < fileSize-1; i++){
            char c = buffer[i];
            char cc = buffer[i+1];
            if (c == 'v' && cc == ' '){
                vertexCount++;
                hasPositions = 1;
            }
            
            if (c == 'v' && cc == 'n'){
                normalCount++;
                hasNormals = 1;
            }
            
            if (c == 'v' && cc == 't'){
                textureCount++;
                hasTextures = 1;
            }
            
            if (c == 'f' && cc == ' '){
                elementCount++;
            }
        }
        
        vector vertices[vertexCount];
        vector normals[normalCount];
        element elements[elementCount];
        int vIndex = 0;
        int nIndex = 0;
        int eIndex = 0;
        
        for (int i = 0; i < fileSize; i++){
            char c = buffer[i];
            char cc = buffer[i+1];
            if (c == 'v' && cc == ' '){
                char *substr = getSubstringUpToDelimiter(i+2, buffer, 10);
                vector vVector = getVectorFromSubstring(substr, " ");
                vertices[vIndex] = vVector;
                vIndex++;
                printf("vertex: %f, %f, %f\n", vVector.x, vVector.y, vVector.z);
                free(substr);
            }
            
            if (c == 'v' && cc == 'n'){
                char *substr = getSubstringUpToDelimiter(i+2, buffer, 10);
                vector vVector = getVectorFromSubstring(substr, " ");
                normals[nIndex] = vVector;
                nIndex++;
                free(substr);
            }
            
            if (c == 'f' && cc == ' '){
                char *substr = getSubstringUpToDelimiter(i+2, buffer, 10);
                char *xStr = strtok(substr, " ");
                char *yStr = strtok(NULL, " ");
                char *zStr = strtok(NULL, " ");
                
                elements[eIndex].xVec = getIVectorFromSubstring(xStr, "/");
                elements[eIndex].yVec = getIVectorFromSubstring(yStr, "/");
                elements[eIndex].zVec = getIVectorFromSubstring(zStr, "/");
                eIndex++;
                free(substr);
            }
        }
        
        if (indexed){
            
        } else {
            
            GLsizei vertexArraySize = sizeof(vector) * elementCount * 3;
            
            mPositions = (GLfloat*)malloc(vertexArraySize);
            mPositionArraySize = vertexArraySize;
            mPositionSize = 3;
            mPositionType = GL_FLOAT;
            if (hasNormals){
                mNormals = (GLfloat*)malloc(vertexArraySize);
                mNormalArraySize = vertexArraySize;
                mNormalSize = 3;
                mNormalType = GL_FLOAT;
            }
            //if (hasTextures)mTextureUV = (GLfloat*)malloc(sizeof(GLfloat) * elementCount * 3);
            for (int i = 0, j = 0; i < elementCount; i++, j+=9){
                mPositions[j] = vertices[elements[i].xVec.x - 1].x;
                mPositions[j+1] = vertices[elements[i].xVec.x - 1].y;
                mPositions[j+2] = vertices[elements[i].xVec.x - 1].z;
                mPositions[j+3] = vertices[elements[i].yVec.x - 1].x;
                mPositions[j+4] = vertices[elements[i].yVec.x - 1].y;
                mPositions[j+5] = vertices[elements[i].yVec.x - 1].z;
                mPositions[j+6] = vertices[elements[i].zVec.x - 1].x;
                mPositions[j+7] = vertices[elements[i].zVec.x - 1].y;
                mPositions[j+8] = vertices[elements[i].zVec.x - 1].z;
                
                if (mNormals){
                    int normal1, normal2, normal3;
                    if (!hasTextures){
                        normal1 = elements[i].xVec.y - 1;
                        normal2 = elements[i].yVec.y - 1;
                        normal3 = elements[i].zVec.y - 1;
                    } else {
                        normal1 = elements[i].xVec.z - 1;
                        normal2 = elements[i].yVec.z - 1;
                        normal3 = elements[i].zVec.z - 1;
                    }
                    
                    mNormals[j] = normals[normal1].x;
                    mNormals[j+1] = normals[normal1].y;
                    mNormals[j+2] = normals[normal1].z;
                    mNormals[j+3] = normals[normal2].x;
                    mNormals[j+4] = normals[normal2].y;
                    mNormals[j+5] = normals[normal2].z;
                    mNormals[j+6] = normals[normal3].x;
                    mNormals[j+7] = normals[normal3].y;
                    mNormals[j+8] = normals[normal3].z;
                }
            }
            
            mNumVertices = elementCount * 3;
        }
        
        printf("vertices: %i\nnormals:%i\nelements:%i\n", vertexCount, normalCount, elementCount);
        
        for (int i = 0; i < elementCount * 9; i+=3){
            printf("%f %f %f\n", mPositions[i], mPositions[i + 1], mPositions[i + 2]);
            printf("%f %f %f\n", mNormals[i], mNormals[i + 1], mNormals[i + 2]);
        }
        
        if (!hasTextures)mTextureUV = NULL;
        if (!hasNormals)mNormals = NULL;
        if (!hasPositions)mPositions = NULL;
        if (!indexed)mElements = NULL;
        
    } else {
        printf("NO FILE!!!!\n");
    }
    
    CFRelease(mainBundle);
    CFRelease(filePath);
    CFRelease(url);
    CFRelease(name);
}

ObjModelLoader::~ObjModelLoader() {
    if (mPositions)free(mPositions);
    if (mNormals)free(mNormals);
    if (mTextureUV)free(mTextureUV);
    if (mElements)free(mElements);
}


