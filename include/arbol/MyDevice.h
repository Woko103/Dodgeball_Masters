#pragma once

#include <MyEntity.h>
#include <iostream>

using namespace std;

class MyDevice
{
public:
    GLFWwindow* window;
    MyDevice();
    int initialize(const char* title);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    int getWindowWidth();
    int getWindowHeight();
};