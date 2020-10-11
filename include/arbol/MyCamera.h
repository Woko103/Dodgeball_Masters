#pragma once

#include <MyEntity.h>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream> 

using namespace glm;
using namespace std;

class MyCamera //  In the future, this class will inherit from MyEntity
{
        Shader* shader; // Main shader copy
        GLFWwindow* window; // Main window copy
        double lastX; // Last cursor position on the X axis
        double lastY; // Last cursor position on the Y axis
        double width;
        double height;

        bool firstMouse; // Boolean to check for the first movement of the mouse inside the window.
        vec3 out_origin;
        vec3 out_direction;
public:
        vec3 position; // Vector that represents the position of the camera
        vec3 direction; // Vector that represents the direction of the camera
        vec3 up; // Vector that points to the +Y of the camera
        vec3 right; // Vector that points to the +X of the camera

        mat4 view; // View matrix of the camera
        mat4 projection; // Projection matrix 

        float yaw; // Yaw of the camera
        float pitch; // Pitch of the camera
        float fov; // Field of view  of the camera
        float cameraSpeed; // Speed of the camera
        float sensitivity; // Sensitivity of the mouse
        float cursorXNorm;
        float cursorYNorm; // Normalized cursor positions

        
        MyCamera(GLFWwindow* mainWindow, Shader* mainShader, int, int);        //MyCamera(const MyCamera&) = delete;
        //MyCamera& operator=(const MyCamera&) = delete;
        //~MyCamera() = default;
        void updatePosition(); // Manages the position of the camera
        static void cursorCallback(GLFWwindow* window, double cursorX, double cursorY);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        void update(int);  // Updates the camera settings and sends them to the shaders 
        bool intersectRay(vec3, vec3, mat4);
        void follow(float, float, float, int);
};