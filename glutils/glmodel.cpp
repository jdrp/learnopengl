#include "glutils/glmodel.h"

#include <stb_image.h>
#include <GL/glext.h>
#include <glm/gtx/quaternion.hpp>

void Model::draw(const Shader &shader) const {
    shader.use();
    shader.setMat4("model", getModelMatrix());
    for (auto &mesh : meshes)
        mesh.draw(shader);
}

void Model::deallocate() {
    for (auto &mesh : meshes) {
        mesh.deallocate();
    }

    meshes.clear();
    for (auto texture : texturesLoaded)
        glDeleteTextures(1, &texture.id);
    texturesLoaded.clear();
}

glm::mat4 Model::getModelMatrix() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model *= glm::toMat4(rotation);
    model = glm::scale(model, scale);
    return model;
}


void Model::loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else vertex.texCoords = glm::vec2(0.0f);

        vertices.push_back(vertex);
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // textures
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Texture> emissionMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emission");
    textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());

    return {vertices, indices, textures};
}

Mesh *Model::getMesh(unsigned int meshIndex) {
    return &meshes[meshIndex];
}

Material *loadMaterial(aiMaterial* mat) {
    Material *material;
    aiColor3D color(0.f, 0.f, 0.f);
    float shininess;

    mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    material->diffuse = glm::vec3(color.r, color.g, color.b);

    mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
    material->ambient = glm::vec3(color.r, color.g, color.b);

    mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
    material->specular = glm::vec3(color.r, color.g, color.b);

    mat->Get(AI_MATKEY_SHININESS, shininess);
    material->shininess = shininess;

    return material;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;

    for (int i = 0; i < material->GetTextureCount(type); i++) {
        aiString path;
        material->GetTexture(type, i, &path);

        bool skip = false;
        for (int j = 0; j < texturesLoaded.size(); j++) {
            if (std::strcmp(texturesLoaded[j].path.data(), path.data) == 0) {
                textures.push_back(texturesLoaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;
            texture.id = load2DTexture(directory + "/" + path.data);
            texture.type = typeName;
            texture.path = path.data;

            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }

    return textures;
}

void Model::setPosition(glm::vec3 position) {
    this->position = position;
}

void Model::setRotation(glm::quat rotation) {
    this->rotation = rotation;
}

void Model::setRotation(glm::vec3 rotation) {
    setRotation(glm::quat(glm::radians(rotation)));
}

void Model::setScale(glm::vec3 scale) {
    this->scale = scale;
}

void Model::setScale(float scale) {
    setScale(glm::vec3(scale));
}

void Model::addPosition(glm::vec3 position) {
    this->position += position;
}

void Model::addRotation(glm::quat rotation) {
    this->rotation *= rotation;
}

void Model::addRotation(glm::vec3 rotation) {
    addRotation(glm::quat(glm::radians(rotation)));
}

void Model::addScale(glm::vec3 scale) {
    this->scale += scale;
}

glm::vec3 Model::getPosition() {
    return position;
}

glm::quat Model::getRotation() {
    return rotation;
}

glm::vec3 Model::getScale() {
    return scale;
}


void drawModels(const std::vector<Model*> &models, const Shader &shader) {
    glStencilMask(0x00);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    for (const Model *model : models) {
        model->draw(shader);
    }
}

void drawOffsetOutline(Model *model, const glm::mat4 &view, const Shader &outlineShader) {
    glm::vec3 originalPosition = model->getPosition();

    for (int i = 0; i < 4; ++i) {
        glm::vec3 viewSpaceOffset(
            (i & 1 ? 1.0f : -1.0f) * 0.01f,
            (i & 2 ? 1.0f : -1.0f) * 0.01f,
            0.0f
        );
        glm::vec3 worldSpaceOffset = glm::inverse(view) * glm::vec4(viewSpaceOffset, 0.0f);
        model->setPosition(originalPosition + worldSpaceOffset);
        model->draw(outlineShader); // Draw with slight offset for dilation effect
    }

    model->setPosition(originalPosition);
}

void drawOutlinedModels(std::vector<Model*> &models, const Shader &shader, const Shader &outlineShader, const float thickness, const OutlineType outlineType) {

    if (outlineType == OUTLINE_COMPLETE_OPAQUE) {
        for (auto model : models) {
            glEnable(GL_STENCIL_TEST);
            glStencilMask(0xFF);
            glClear(GL_STENCIL_BUFFER_BIT);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);

            model->draw(shader);

            glStencilMask(0x00);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

            glLineWidth(thickness);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            model->draw(outlineShader);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    else if (outlineType == OUTLINE_OUTER_SEETHROUGH) {
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);

        for (auto model : models)
            model->draw(shader);

        glStencilMask(0x00);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glDisable(GL_DEPTH_TEST);

        glLineWidth(thickness);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (auto model : models)
            model->draw(outlineShader);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_DEPTH_TEST);
    }
}

Model *getPlaneModel(const std::string &texturePath, float width, float height) {
    std::vector<Mesh> meshes = {getPlaneMesh(texturePath, width, height)};
    return new Model(meshes);
}
