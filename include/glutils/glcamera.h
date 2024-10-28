#ifndef GLCAMERA_H
#define GLCAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
private:

    bool firstMouse;
    float lastX, lastY;
    float yaw = -90.0f, pitch = 0.0f;
    float aspectRatio;
    float near = 0.1f, far = 100.0f;
    bool mouseCaptured = false;

public:

    glm::vec3 position;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    enum Controls { GLCAMERA_WASD, GLCAMERA_ARROWS, GLCAMERA_WASDARROWS, GLCAMERA_FIXED };
    float sensitivity = 0.1f;
    float velocity = 2.0f;
    float fov = 45.0f;
    Controls moveKeys = GLCAMERA_FIXED;
    bool relativeMovement = true;
    bool allowVerticalMovement = false;

    int displayWidth, displayHeight;

    Camera(int displayWidth, int displayHeight, glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f));

    void resizeDisplay(int width, int height);

    void captureCursor(GLFWwindow *window);
    void releaseCursor(GLFWwindow *window);
    void toggleCursor(GLFWwindow *window);

    void mouseCallback(double xPos, double yPos);
    void keyCallback(GLFWwindow *window, float deltaTime);
    void scrollCallback(double yoffset);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
};

#endif //GLCAMERA_H
