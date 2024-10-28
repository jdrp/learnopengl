#ifndef GLMODEL_H
#define GLMODEL_H

#include <vector>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glshader.h"
#include "glmesh.h"
#include "glutils.h"


enum OutlineType {
    OUTLINE_COMPLETE_OPAQUE,
    OUTLINE_OUTER_SEETHROUGH
};

class Model
{
public:

    explicit Model(const std::string &path) {
        loadModel(path);
    }
    explicit Model(const std::vector<Mesh> &meshes) {
        this->meshes = meshes;
    }
    void draw(const Shader &shader) const;
    void deallocate();


    void setPosition(glm::vec3 position);
    void setRotation(glm::quat rotation);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scale);
    void setScale(float scale);

    void addPosition(glm::vec3 position);
    void addRotation(glm::quat rotation);
    void addRotation(glm::vec3 rotation);
    void addScale(glm::vec3 scale);

    glm::vec3 getPosition();
    glm::quat getRotation();
    glm::vec3 getScale();

    Mesh *getMesh(unsigned int meshIndex);

    [[nodiscard]] glm::mat4 getModelMatrix() const;

private:

    glm::vec3 position = glm::vec3(1.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    std::vector<Mesh> meshes;
    std::string directory;

    std::vector<Texture> texturesLoaded;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

Model *getPlaneModel(const std::string &texturePath = "", float width = 2.0f, float height = 2.0f);
void drawModels(const std::vector<Model*> &models, const Shader &shader);
void drawOutlinedModels(std::vector<Model*> &models, const Shader &shader, const Shader &outlineShader, float thickness, const OutlineType outlineType = OUTLINE_OUTER_SEETHROUGH);

#endif //GLMODEL_H
