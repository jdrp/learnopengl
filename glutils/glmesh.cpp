#include "glutils/glmesh.h"

#include <iostream>
#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(2, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
}

void Mesh::draw(const Shader &shader) const {
    shader.use();
    unsigned int nDiffuse = 0;
    unsigned int nSpecular = 0;
    unsigned int nEmission = 0;

    setDefaultBlackTexture();
    shader.setInt("material.texture_diffuse0", 0);
    shader.setInt("material.texture_specular0", 0);
    shader.setInt("material.texture_emission0", 0);

    for (int i = 0; i < textures.size(); i++) {
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            name += std::to_string(nDiffuse++);
        else if (name == "texture_specular")
            name += std::to_string(nSpecular++);
        else if (name == "texture_emission")
            name += std::to_string(nEmission++);

        glActiveTexture(GL_TEXTURE0 + i + 1);  // skip default black texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        shader.setInt("material." + name, i + 1);
    }
    shader.setFloat("material.shininess", 1.0f);
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

unsigned int Mesh::setTexture(unsigned int textureIndex, Texture &texture) {
    if (textures.size() > textureIndex) {
        textures[textureIndex] = texture;
        return textureIndex;    
    }
    textures.push_back(texture);
    return textures.size() - 1;
}


void Mesh::deallocate() {
    vertices.clear();
    indices.clear();
    textures.clear();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


std::vector<Vertex> getPlaneVertices(float width, float height) {
    const std::vector<float> rawVertices = {
        //        position                       normal          texture
        -width/2.0f,  height/2.0f,  0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        -width/2.0f, -height/2.0f,  0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         width/2.0f, -height/2.0f,  0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
         width/2.0f,  height/2.0f,  0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f
    };

    std::vector<Vertex> vertices;
    for (size_t i = 0; i < rawVertices.size(); i += 8) {
        Vertex vertex;
        vertex.position = glm::vec3(rawVertices[i], rawVertices[i + 1], rawVertices[i + 2]);
        vertex.normal = glm::vec3(rawVertices[i + 3], rawVertices[i + 4], rawVertices[i + 5]);
        vertex.texCoords = glm::vec2(rawVertices[i + 6], rawVertices[i + 7]);
        vertices.push_back(vertex);
    }

    return vertices;
}

Mesh getPlaneMesh(const std::string &texturePath, const float width, const float height) {
    std::vector<Texture> textures;
    if (!texturePath.empty()) {
        Texture texture;
        texture.id = load2DTexture(texturePath);
        if (texture.id == 0) {
            std::cerr << "Failed to load texture: " << texturePath << std::endl;
        }
        texture.path = texturePath;
        texture.type = "texture_diffuse";
        textures.push_back(texture);
    }

    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3};

    std::vector<Vertex> vertices = getPlaneVertices(width, height);

    return {vertices, indices, textures};
}