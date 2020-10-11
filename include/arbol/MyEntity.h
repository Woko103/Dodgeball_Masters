#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class MyEntity {

public:
    // CONSTRUCTOR
    MyEntity(){}

    // DESTRUCTOR
    ~MyEntity(){}

    // VIRTUAL FUNCTIONS
    virtual void draw(glm::mat4 matrix);
};
