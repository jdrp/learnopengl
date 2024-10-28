#ifndef GLMESH_H
#define GLMESH_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "glutils/glshader.h"
#include "glutils/glutils.h"
#include "glutils/gltexture.h"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures);
    void draw(const Shader &shader) const;
    void deallocate();
    unsigned int setTexture(unsigned int textureIndex, Texture &texture);

private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

Mesh getPlaneMesh(const std::string &texturePath, float width, float height);

#endif //GLMESH_H
