#ifndef GLSHADER_H
#define GLSHADER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:

    static std::string preprocessShaderCode(const std::string& code, const std::unordered_map<std::string, std::string>& constants = {});

public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath, const std::unordered_map<std::string, std::string>& constants = {});
    // use/activate the shader
    void use() const;
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setFloatVector(const std::string &name, std::vector<float> values) const;
    void deleteShader() const;
};

#endif //GLSHADER_H
