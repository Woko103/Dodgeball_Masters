#include <MyLight.h>

/*
Constructor
@ shader : Reference to the shader the light will be attached to
@ camera : Reference to the current active camera
*/
MyLight::MyLight(Shader* shader, MyCamera* camera, GLFWwindow* window)
{
        this->shader = shader; // Saving reference to the shader
        this->camera = camera; // Saving reference to the camera
        this->window = window; // Saving reference to the window

        model = mat4(1.0f); // Initializing the light's model matrix
}

/*
Creation of a Point Light
@ lightPosition: Initial position for the Point Light
*/
void MyLight::createPointLight(vec3 lightPosition)
{
        position = lightPosition; // Save the initial position for the light
        model = translate(model, position); // Updating position of the light's model matrix


        ambient = vec3(0.8f); // Ambient lighting initial values
        diffuse = vec3(0.5f); // Diffuse lighting initial values
        specular = vec3(0.5f); // Specular lighting initial values
        constant = 1.0f; // Kc for attenuation
        linear = 0.022f; // Kl for attenuation
        quadratic = 0.0019f; // Kq for attenuation

        vertices =
        {       // Position
            -0.5f, 0.0f, 0.5f,
            0.5 , 0.0f, 0.5f,     // Front face
            0.0f, 0.5f, 0.0f,

            0.5f, 0.0f, 0.5f,
            0.0f, 0.0f, -0.5f,     // Right face
            0.0f, 0.5f, 0.0f,

            -0.5f, 0.0f, 0.5f,
            0.0f, 0.0f, -0.5f,     // Left face
            0.0f, 0.5f, 0.0f,

            -0.5f, 0.0f, 0.5f,
            0.5f, 0.0f, 0.5f,      // Back face
            0.0f, 0.0f, -0.5f
        };

        glGenVertexArrays(1, &VAO); // Create the Vertex Array Object
        glGenBuffers(1, &VBO); // Create the Vertex Buffer Object

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Inserting all the vertices data inside the VBO buffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        // Retrieving vertex's position:
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
}

void MyLight::generateShadows()
{
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGT = 1024;

    // Creating framebuffer for rendering the depth map
    glGenFramebuffers(1, &depthMapFBO);

    // Create texture to use as the framebuffer's depth buffer
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

// Method for updating the light's position
void MyLight::updatePosition()
{
    vec3 newPosition(0);

    if (glfwGetKey(window, GLFW_KEY_RIGHT)) // Move right
        newPosition = vec3(0.1, 0, 0);
    else if (glfwGetKey(window, GLFW_KEY_LEFT)) // Move left
        newPosition = vec3(-0.1, 0, 0);
    else if (glfwGetKey(window, GLFW_KEY_UP)) // Move forwards
        newPosition = vec3(0, 0, -0.1);
    else if (glfwGetKey(window, GLFW_KEY_DOWN)) // Move downwards
        newPosition = vec3(0, 0, 0.1);

    position += newPosition; // Updating the position
    model = translate(model, newPosition); // Updating the model matrix
}


// Method for updating the shader attached to our type of light with new data
void MyLight::updateLight()
{
    // Updating the Light Shader, which will be affecting the rest of the objects in the scene
    shader->setBool("isLight", true);
    shader->setVec3("light.position", position);
    shader->setVec3("viewPos", camera->position);
    shader->setMat4("model", model);

    shader->setVec3("light.ambient", ambient);
    shader->setVec3("light.diffuse", diffuse);
    shader->setVec3("light.specular", specular);
    shader->setFloat("light.constant", constant);
    shader->setFloat("light.linear", linear);
    shader->setFloat("light.quadratic", quadratic);

    updatePosition();

    updateValues();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
}

void MyLight::updateValues(){
  if (glfwGetKey(window, GLFW_KEY_1)){
    ambient += 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_2)){
    ambient -= 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_3)){
    diffuse += 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_4)){
    diffuse -= 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_5)){
    specular += 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_6)){
    specular -= 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_7)){
    constant += 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_8)){
    constant -= 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_9)){
    linear += 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_0)){
    linear -= 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_O)){
    quadratic += 0.01;
  }
  else if (glfwGetKey(window, GLFW_KEY_L)){
    quadratic -= 0.01;
  }
}
