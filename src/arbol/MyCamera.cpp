#include <MyCamera.h>

MyCamera::MyCamera(GLFWwindow* mainWindow, Shader* mainShader, int p_width, int p_height)
{
        // Camera settings

        cameraSpeed = 0.05f;
        sensitivity = 0.5;
        yaw = -90.0f;
        pitch = 0.0f;
        fov = 45.0f;
        lastX = 400.0f;
        lastY = 300.0f;
        firstMouse = false;

        // Establishing camera's vectors

        position = vec3(0.0f, 4.0f, 4.0f );
        direction = vec3(0.0f, -0.75f, -0.75f);
        up = vec3(0.0f, 1.0f, 0.0f);
        // Window
        window = mainWindow;
        width = p_width;
        height = p_height;

        // Shader
        shader = mainShader;

        // Callback functions
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetCursorPosCallback(window, cursorCallback);
        glfwSetWindowUserPointer(window, this);
        //createRay();
}
// Zoom of the camera, changing the projection matrix
void MyCamera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
        // Get the pointer stored  as the User Pointer, which is a pointer to our main camera
        MyCamera* camera = static_cast<MyCamera*>(glfwGetWindowUserPointer(window));

        camera->fov -= yoffset;

        if (camera->fov > 45.0f)
                camera->fov = 45.0f;

        if (camera->fov < 1.0f)
                camera->fov = 1.0f;
}

// Rotation and direction of the camera, changing the view matrix
void MyCamera::cursorCallback(GLFWwindow* window, double cursorX, double cursorY)
{
        MyCamera* camera = static_cast<MyCamera*>(glfwGetWindowUserPointer(window));
        camera->cursorXNorm = ((float)cursorX/(float)camera->width - 0.5f) * 2.0f;
        camera->cursorYNorm = ((float)cursorY/(float)camera->height - 0.5f) * 2.0f;
}

bool MyCamera::intersectRay(vec3 colMin, vec3 colMax, mat4 modelMat){
        return false;
}

void MyCamera::updatePosition()
{
        // Check for inputs
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // LEFT
                position -= cameraSpeed * cross(direction, up);

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // RIGHT
                position += cameraSpeed * cross(direction, up);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // FORWARDS
                position += cameraSpeed * direction;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // BACKWARDS
                position -= cameraSpeed * direction;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // UP
                position += cameraSpeed * up;

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // DOWN
                position -= cameraSpeed * up;
}

void MyCamera::update(int multiplayerId)
{
        // VIEW MATRIX
                // Update the camera's view matrix containing the position, the target and the upVector
                if(multiplayerId == 2){
                    direction = vec3(0.0f, -0.75f, 0.75f);
                }
                view = lookAt(position, position + direction, up);

                // Sends camera's view to the shaders
                unsigned int viewLocation = glGetUniformLocation(shader->id, "view");
                glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));


        // PROJECTION MATRIX
                projection = perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
                unsigned int projectionLocation = glGetUniformLocation(shader->id, "projection");
                glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void MyCamera::follow(float X, float Y, float Z, int multiplayerId){
        position.x = X;
        position.y = Y + 4.0f;
        if(multiplayerId == 2)
            position.z = Z - 4.0f;
        else
            position.z = Z + 4.0f;
}
