#ifndef GLUTILS_H
#define GLUTILS_H

#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/detail/type_mat4x2.hpp>
#include <iostream>

#include "stb_image.h"
#include "glcamera.h"

GLFWwindow* setupGlfwOpenGl(int width, int height, const std::string& title, Camera *camera, const std::string& glVersion = "3.3");

#endif // GLUTILS_H
