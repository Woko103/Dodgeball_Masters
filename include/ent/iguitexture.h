#pragma once

#include <MyGraphicsEngine.h>

struct IGuiTexture
{
    const char* texturePath;
    int width;
    int height;
    GLuint texture;
};