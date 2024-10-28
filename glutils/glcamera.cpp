#include <iostream>
#include <glutils/glcamera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(int displayWidth, int displayHeight, glm::vec3 position) {
    lastX = (float)displayWidth / 2.0f;
    lastY = (float)displayHeight / 2.0f;
    this->position = position;
    resizeDisplay(displayWidth, displayHeight);
}

void Camera::resizeDisplay(int width, int height) {
    this->displayWidth = width;
    this->displayHeight = height;
    aspectRatio = (float)width / (float)height;
}

void Camera::captureCursor(GLFWwindow *window) {
    firstMouse = true;
    mouseCaptured = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Camera::releaseCursor(GLFWwindow *window) {
    mouseCaptured = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void Camera::toggleCursor(GLFWwindow *window) {
    if (mouseCaptured) releaseCursor(window);
    else captureCursor(window);
}


void Camera::mouseCallback(double xPos, double yPos)
{
    if (!mouseCaptured) return;
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
  
    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw   += xOffset;
    pitch += yOffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

void Camera::keyCallback(GLFWwindow *window, float deltaTime) {

    // MOVEMENT
    const float movementSpeed = velocity * deltaTime; // adjust accordingly

    bool forward = false, backward = false, rightward = false, leftward = false;
    if (moveKeys == GLCAMERA_WASD || moveKeys == GLCAMERA_WASDARROWS) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            forward = true;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            backward = true;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            leftward = true;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            rightward = true;
    }
    if (moveKeys == GLCAMERA_ARROWS || moveKeys == GLCAMERA_WASDARROWS) {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            forward = true;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            backward = true;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            leftward = true;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            rightward = true;
    }

    if (relativeMovement) {
        if (forward)
            position += movementSpeed * front;
        if (backward)
            position -= movementSpeed * front;
    }
    else {
        const glm::vec3 worldFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        if (forward)
            position += movementSpeed * worldFront;
        if (backward)
            position -= movementSpeed * worldFront;

    }
    if (leftward)
        position -= glm::normalize(glm::cross(front, up)) * movementSpeed;
    if (rightward)
        position += glm::normalize(glm::cross(front, up)) * movementSpeed;

    if (allowVerticalMovement) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            position += movementSpeed * up;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            position -= movementSpeed * up;
    }

}

void Camera::scrollCallback(double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

