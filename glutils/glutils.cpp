#include "glutils/glutils.h"


void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (Camera *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window))) {
        camera->resizeDisplay(width, height);
    }
    // std::cout << "Framebuffer size changed: width=" << width << ", height=" << height << std::endl;
}

GLFWwindow* setupGlfwOpenGl(const int width, const int height, const std::string& title, Camera *camera, const std::string& glVersion) {

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(glVersion[0] - '0'));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(glVersion[2] - '0'));
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    stbi_set_flip_vertically_on_load(true);

    glfwSetWindowUserPointer(window, camera);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return window;
}