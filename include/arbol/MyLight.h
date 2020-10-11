#pragma once

#include <shader.h>
#include <MyCamera.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

using namespace glm;

class MyLight
{
    Shader* shader; // Reference to the shader
    GLFWwindow* window; // Reference to the main window
    MyCamera* camera; // Reference to the camera

    vector<float> vertices; // Array used for a physical display of the light
    mat4 model; // Transformation matrix of our Light

    unsigned int VAO;
    unsigned int VBO;
    unsigned int depthMapFBO;
    unsigned int depthMap;

    float constant; // Constant value for our attenuation formula (Point Light)
    float linear; // Linear value for our attenuation formula (Point Light)
    float quadratic; // Quadratic value for our attenuation formula (Point Light)
public:
    vec3 direction; // Direction of our light (Directional Light)
    vec3 position; // Position of our light (Point Light)
    vec3 ambient; // Vector representing the ambient light
    vec3 diffuse; // Vector representing the diffuse light
    vec3 specular; // Vector representing the specular light

    MyLight(Shader* shader, MyCamera* camera, GLFWwindow* window);
    //void createDirectionalLight(vec3 lightDirection);
    void createPointLight(vec3 lightPosition);
    void generateShadows();
    void updatePosition();
    void updateLight();

    void updateValues();
};
