#include "glutils/glshader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include <glad/glad.h>


std::string Shader::preprocessShaderCode(const std::string& code, const std::unordered_map<std::string, std::string>& constants) {
    std::string processedCode = code;

    for (const auto& constant : constants) {
        std::string placeholder = "#define " + constant.first;
        std::string replacement = placeholder + " " + constant.second;

        // use regex to match and replace the entire line that defines the constant
        std::regex defineRegex(placeholder + "\\s+\\S+");

        // if the constant is found, replace it
        if (std::regex_search(processedCode, defineRegex)) {
            processedCode = std::regex_replace(processedCode, defineRegex, replacement);
        } else {
            std::cout << "ERROR::SHADER::CONSTANT_NOT_FOUND::" + constant.first + "\n";
        }
    }

    return processedCode;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const std::unordered_map<std::string, std::string>& constants) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        std::cout << "Loading vertex shader from: " << vertexPath << std::endl;
        std::cout << "Loading fragment shader from: " << fragmentPath << std::endl;

        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // preprocess constants
        vertexCode = preprocessShaderCode(vertexCode, constants);
        fragmentCode = preprocessShaderCode(fragmentCode, constants);

        // std::cout << "Vertex Shader Code:\n" << vertexCode << std::endl;
        // std::cout << "Fragment Shader Code:\n" << fragmentCode << std::endl;
    } catch(std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    } else {
        std::cout << "Vertex shader compiled successfully." << std::endl;
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    } else {
        std::cout << "Fragment shader compiled successfully." << std::endl;
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    } else {
        std::cout << "Shader program linked successfully." << std::endl;
    }

    // delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setFloatVector(const std::string &name, std::vector<float> values) const {
    glUniform1fv(glGetUniformLocation(ID, name.c_str()), values.size(), values.data());
}


void Shader::deleteShader() const {
    glDeleteProgram(ID);
}