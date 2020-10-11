#pragma once

#include <MyNode.h>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream> 

using namespace glm;
using namespace std;

class MyCollider //  In the future, this class will inherit from MyEntity
{
    char* name;

    MyNode* parent;

    float xSize, ySize, zSize;
    float xOffset, yOffset, zOffset;

    unsigned int VAO, VBO;

public:

    MyCollider(char* name, MyNode* parent, float xSize, float ySize, float zSize, float xOffset, float yOffset, float zOffset);

    // Getters
    char* getName()
    {
        return name;
    }

    MyNode* getParent()
    {
        return parent;
    }

    float getXSize()
    {
        return xSize;
    }

    float getYSize()
    {
        return ySize;
    }

    float getZSize()
    {
        return zSize;
    }

    float getXOffset()
    {
        return xOffset;
    }

    float getYOffset()
    {
        return yOffset;
    }

    float getZOffset()
    {
        return zOffset;
    }

    // Draw method
    void draw(mat4 matrix);
};