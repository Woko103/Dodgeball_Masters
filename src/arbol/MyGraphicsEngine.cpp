#include <MyGraphicsEngine.h>

MyGraphicsEngine::MyGraphicsEngine()
{
    device = new MyDevice(); // Device
}

// Initialize window's settings
void MyGraphicsEngine::initDevice(const char* title)
{
    device->initialize(title); // Initializing device
    lightingShader = new Shader("opengl/shaders/lightingShader.vs", "opengl/shaders/lightingShader.fs"); // Shaders
}

// Clear color buffer function
void MyGraphicsEngine::clear()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Poll events function
void MyGraphicsEngine::pollEvents()
{
    glfwPollEvents();
}

// Swap buffers function
void MyGraphicsEngine::swapBuffers()
{
    if(device)
        glfwSwapBuffers(device->window);
}

// Get and set time functions
double MyGraphicsEngine::getTime()
{
    return glfwGetTime();
}

void MyGraphicsEngine::setTime(double time)
{
    glfwSetTime(time);
}

/* GRAPHICS ENGINE INPUT FUNCTIONS */
// KeyBoard input
bool MyGraphicsEngine::getKey(int keyCode)
{
    if(device)
        return glfwGetKey(device->window, keyCode) == GLFW_PRESS;
    
    return false;
}

// Mouse input
bool MyGraphicsEngine::getMouseButton(int buttonCode)
{
    if(device)
        return glfwGetMouseButton(device->window, buttonCode) == GLFW_PRESS;

    return false;
}

// Joystick input
const float* MyGraphicsEngine::getJoystickAxes(int joystickCode, int* count)
{
    return glfwGetJoystickAxes(joystickCode, count);
}

const unsigned char* MyGraphicsEngine::getJoystickButtons(int joystickCode, int* count)
{
    return glfwGetJoystickButtons(joystickCode, count);
}

bool MyGraphicsEngine::joystickPresent(int joystickCode)
{
    return glfwJoystickPresent(joystickCode);
}

/* WINDOW MANAGEMENT FUNCTIONS */
bool MyGraphicsEngine::windowShouldClose()
{
    if(device)
        return glfwWindowShouldClose(device->window);
}

void MyGraphicsEngine::terminate()
{
    glfwTerminate();
}

/* TEXTURE LOAD METHOD */
// Loads texture from file
bool MyGraphicsEngine::loadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}
