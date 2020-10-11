#pragma once

#include <MyDevice.h>
#include <MyNode.h>
#include <MyCamera.h>
#include <MyModel.h>
#include <MyLight.h>
#include <MyCollider.h>

class MyGraphicsEngine
{    
/*
    Interface class of the engine. 
    Every call to a engine function must pass through this class.
*/    
public:
    Shader* lightingShader;
    MyDevice* device; // Reference to our device

    MyGraphicsEngine();

    // Initialize window's settings
    void initDevice(const char* title);

    // Clear color buffer function
    void clear();

    // Poll events function
    void pollEvents();

    // Swap buffers function
    void swapBuffers();

    // Get and Set time functions
    double getTime();
    void setTime(double time);

    /* GRAPHICS ENGINE INPUT FUNCTONS */
    bool getKey(int keyCode);
    bool getMouseButton(int buttonCode);
    const float* getJoystickAxes(int joystickCode, int* count);
    const unsigned char* getJoystickButtons(int joystickCode, int* count);
    bool joystickPresent(int joystickCode);

    /* WINDOW MANAGEMENT FUNCTIONS */
    bool windowShouldClose();
    void terminate();

    /* TEXTURE LOAD METHOD*/
    // Load a texture from file
    bool loadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
};