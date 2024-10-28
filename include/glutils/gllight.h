#ifndef GLLIGHT_H
#define GLLIGHT_H

#include <glm/fwd.hpp>

#include "glcamera.h"
#include "glshader.h"

struct Light {
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(0.5f);

    virtual void setLightInShader(const Shader &shader, int lightIndex) const = 0;
};

struct DirectionalLight : Light {
    glm::vec3 direction;

    void setLightInShader(const Shader &shader, int lightIndex) const override;
};


struct PointLight : Light {
    glm::vec3 position;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    bool attachedToCamera = false;

    void attachTo(Camera camera);
    void setLightInShader(const Shader &shader, int lightIndex) const override;
};


struct SpotLight : Light {
    glm::vec3 position;
    glm::vec3 direction;

    float constant = 1.0f;
    float linear = 0.009f;
    float quadratic = 0.0032f;

    float innerCutoff = 10.0f;
    float outerCutoff = 12.5f;

    bool attachedToCamera = false;

    void attachTo(Camera camera);
    void setLightInShader(const Shader &shader, int lightIndex) const override;
};


#endif //GLLIGHT_H
