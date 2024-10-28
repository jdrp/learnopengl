#include "glutils/gllight.h"
#include <iostream>

void DirectionalLight::setLightInShader(const Shader &shader, int lightIndex) const {
    std::string object = "directionalLights[" + std::to_string(lightIndex) + "]";
    shader.setVec3(object + ".direction", this->direction);
    shader.setVec3(object + ".ambient", this->ambient);
    shader.setVec3(object + ".diffuse", this->diffuse);
    shader.setVec3(object + ".specular" ,this->specular);
}


void PointLight::setLightInShader(const Shader &shader, int lightIndex) const {
    std::string object = "pointLights[" + std::to_string(lightIndex) + "]";
    shader.setVec3(object + ".position", this->position);
    shader.setVec3(object + ".ambient", this->ambient);
    shader.setVec3(object + ".diffuse", this->diffuse);
    shader.setVec3(object + ".specular" ,this->specular);
    shader.setFloat( object + ".constant", this->constant);
    shader.setFloat( object + ".linear", this->linear);
    shader.setFloat( object + ".quadratic", this->quadratic);
}


void SpotLight::setLightInShader(const Shader &shader, int lightIndex) const {
    std::string object = "spotLights[" + std::to_string(lightIndex) + "]";
    shader.setVec3(object + ".position", this->position);
    shader.setVec3(object + ".direction", this->direction);
    shader.setVec3(object + ".ambient", this->ambient);
    shader.setVec3(object + ".diffuse", this->diffuse);
    shader.setVec3(object + ".specular" ,this->specular);
    shader.setFloat( object + ".constant", this->constant);
    shader.setFloat( object + ".linear", this->linear);
    shader.setFloat( object + ".quadratic", this->quadratic);
    shader.setFloat(object + ".cosInnerCutoff", glm::cos(glm::radians(this->innerCutoff)));
    shader.setFloat(object + ".cosOuterCutoff", glm::cos(glm::radians(this->outerCutoff)));
}

void PointLight::attachTo(Camera camera) {
    this->position = camera.position;
}

void SpotLight::attachTo(Camera camera) {
    this->position = camera.position;
    this->direction = camera.front;
}
