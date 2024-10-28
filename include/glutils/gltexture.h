#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "glutils/glutils.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};


unsigned int load2DTexture(const std::string &path, bool flip = true);
Texture generateEmptyTexture(int width, int height);
void resizeEmptyTexture(unsigned int id, int newWidth, int newHeight);
void setDefaultBlackTexture();
unsigned int loadCubemapTexture(std::vector<std::string> &faces);
unsigned int loadCubemapTexture(std::string dir, std::string format = "jpg");
unsigned int createCubemapVOs();

#endif //GLTEXTURE_H
