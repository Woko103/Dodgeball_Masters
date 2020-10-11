#include <MyDevice.h>

MyDevice::MyDevice()
{
}

int MyDevice::initialize(const char* title)
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    int width = mode->width;
    int height = mode->height;

    // Create our window and make it to current context, main window
    window = glfwCreateWindow(width, height, title, monitor, NULL);

    if (window == NULL)
    {
            cout << "Failed to create the window" << endl;
            glfwTerminate();
            return -1;
    }
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    // Load all OpenGL function pointers
    if (GLEW_OK != err)
    {
        cout << "Failed to initialize GLEW" << endl;
        return -1;
    }

    // Enable z buffer and blending for alpha
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Function callback everytime the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Setting initial Viewport size to be the same as our initial window size
    glViewport(0, 0, width, height);
}

void MyDevice::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // We are setting the Viewport size to the Window size
    glViewport(0, 0, width, height);
}

int MyDevice::getWindowWidth()
{
    if(!window) {return 0;}

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    return width;
}

int MyDevice::getWindowHeight()
{
    if(!window) {return 0;}

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    return height;
}
