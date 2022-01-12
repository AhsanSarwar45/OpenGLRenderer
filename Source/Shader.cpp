#include "Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "ResourceManager.hpp"
#include "Utilities/Hash.hpp"

using namespace ShaderInternal;

ShaderProgram LoadShader(const std::vector<std::string>& shaderStagePaths, const char* name, bool cameraTransform)
{
    std::vector<ShaderStage> shaderStages;
    for (const auto& shaderStagePath : shaderStagePaths)
    {
        size_t extLoc = shaderStagePath.find_last_of('.');
        if (extLoc == std::string::npos)
        {
            return 999; // TODO return fallback shader
        }

        GLenum shaderType;

        std::string ext = shaderStagePath.substr(extLoc + 1);
        if (ext == "vert") // TODO check for multiplt file name conventions
            shaderType = GL_VERTEX_SHADER;
        else if (ext == "frag")
            shaderType = GL_FRAGMENT_SHADER;
        else if (ext == "geom")
            shaderType = GL_GEOMETRY_SHADER;
        else if (ext == "tesc")
            shaderType = GL_TESS_CONTROL_SHADER;
        else if (ext == "tese")
            shaderType = GL_TESS_EVALUATION_SHADER;
        else if (ext == "comp")
            shaderType = GL_COMPUTE_SHADER;
        else
            return 999; // TODO return fallback shader

        shaderStages.push_back({.path = shaderStagePath, .type = shaderType});
    }

    return LoadShaderStages(shaderStages, name, cameraTransform);
}

// ShaderProgram LoadShader(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath, const char* name,
//                          bool cameraTransform)
// {
//     return LoadShaders({vertexShaderPath.string(), fragmentShaderPath.string()}, name, cameraTransform);
// }

void ShaderSetBool(ShaderProgram shaderProgram, const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}
void ShaderSetInt(ShaderProgram shaderProgram, const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void ShaderSetFloat(ShaderProgram shaderProgram, const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void ShaderSetInt4(ShaderProgram shaderProgram, const std::string& name, int x, int y, int z, int w)
{
    glUniform4i(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}
void ShaderSetFloat4(ShaderProgram shaderProgram, const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}
void ShaderSetInt3(ShaderProgram shaderProgram, const std::string& name, int x, int y, int z)
{
    glUniform3i(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}
void ShaderSetFloat3(ShaderProgram shaderProgram, const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}
void ShaderSetFloat3(ShaderProgram shaderProgram, const std::string& name, const glm::vec3& vec)
{
    glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), vec.x, vec.y, vec.z);
}
void ShaderSetMat4(ShaderProgram shaderProgram, const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void UseShaderProgram(const ShaderProgram shaderProgram) { glUseProgram(shaderProgram); }

namespace ShaderInternal
{
ShaderProgram LoadShaderStages(std::vector<ShaderStage>& shaderStages, const char* name, bool cameraTransform)
{
    ShaderProgram shaderProgram = glCreateProgram();

    // find references to existing shaders, and create ones that didn't exist previously.
    for (auto& shaderData : shaderStages)
    {
        ShaderStageId shaderStageId = CreateShaderStage(shaderData.type);
        CompileShaderStage(shaderStageId, shaderData.type, ParseShaderStage(shaderData.path).c_str());

        shaderData.id = shaderStageId;

        glAttachShader(shaderProgram, shaderStageId);

        shaderData.path.make_preferred();

        ResourceManager::GetInstance().AddShader(shaderProgram, shaderData.path, shaderStages);
    }

    // TODO safety issues

    LinkProgram(shaderProgram);
    ValidateProgram(shaderProgram);

    for (auto& shaderData : shaderStages)
    {
        glDetachShader(shaderProgram, shaderData.id);
        DeleteShaderStage(shaderData.id);
    }

    glObjectLabel(GL_PROGRAM, shaderProgram, strlen(name), name);

    if (cameraTransform)
    {
        glUniformBlockBinding(shaderProgram, glGetUniformBlockIndex(shaderProgram, "Camera"), 0);
    }

    return shaderProgram;
}

bool ReloadShaderStage(const ShaderProgram shaderProgram, std::vector<ShaderStage>& shaderStages)
{
    for (auto& shaderData : shaderStages)
    {
        ShaderStageId shaderStageId = CreateShaderStage(shaderData.type);
        CompileShaderStage(shaderStageId, shaderData.type, ParseShaderStage(shaderData.path).c_str());

        shaderData.id = shaderStageId;

        glAttachShader(shaderProgram, shaderStageId);
    }

    // TODO safety issues
    LinkProgram(shaderProgram);
    ValidateProgram(shaderProgram);

    for (auto& shaderData : shaderStages)
    {
        glDetachShader(shaderProgram, shaderData.id);
        DeleteShaderStage(shaderData.id);
    }

    // CompileShaderStage(shaderStage.id, shaderStage.type, ParseShaderStage(shaderStage.path).c_str());

    // PrintShaderStageSource(shaderStage.id);

    // glAttachShader(shaderProgram, shaderStage.id);

    // if (!LinkProgram(shaderProgram))
    // {
    //     glDetachShader(shaderProgram, shaderStage.id);
    //     DeleteShaderStage(shaderStage.id);
    //     return false;
    // }
    // glDetachShader(shaderProgram, shaderStage.id);
    // DeleteShaderStage(shaderStage.id);
    // if (!ValidateProgram(shaderProgram))
    // {
    //     return false;
    // }

    // return true;
}
bool LinkProgram(const ShaderProgram shaderProgram)
{
    glLinkProgram(shaderProgram);

    int result;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(shaderProgram, length, &length, message);
        std::cout << "Failed to link program (id: " << shaderProgram << ")\n";
        std::cout << message << "\n";
        return false;
    }

    return true;
}

bool ValidateProgram(const ShaderProgram shaderProgram)
{

    glValidateProgram(shaderProgram);

    int result;
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(shaderProgram, length, &length, message);
        std::cout << "Failed to validate program (id: " << shaderProgram << ")\n";
        std::cout << message << "\n";
        return false;
    }

    return true;
}

std::string ParseShaderStage(const std::filesystem::path& path)
{
    std::ifstream stream(path);
    if (!stream.is_open())
    {
        std::cout << "Failed to open file " << path << "\n";
    }

    std::string       line;
    std::stringstream ss;

    while (getline(stream, line))
    {
        ss << line << "\n";
    }

    return ss.str();
}

ShaderStageId CreateShaderStage(const ShaderType type) { return glCreateShader(type); }

bool CompileShaderStage(const ShaderStageId shaderStageId, ShaderType type, std::string source)
{
    const char* src = source.c_str();
    glShaderSource(shaderStageId, 1, &src, nullptr);
    glCompileShader(shaderStageId);

    int result;
    glGetShaderiv(shaderStageId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(shaderStageId, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(shaderStageId, length, &length, message);
        std::cout << "Failed to compile shader (id: " << shaderStageId << ")\n ";
        std::cout << message << "\n";
        SetToFallback(shaderStageId, type);
        return false;
    }

    return true;
}

void PrintShaderStageSource(const ShaderStageId shaderStage)
{
    int length;
    glGetShaderiv(shaderStage, GL_SHADER_SOURCE_LENGTH, &length);
    char* source = (char*)alloca(length * sizeof(char));
    glGetShaderSource(shaderStage, length, &length, source);

    std::cout << source;
}

void SetToFallback(const ShaderStage shaderStage) { SetToFallback(shaderStage.id, shaderStage.type); }

void SetToFallback(const ShaderStageId shaderStageId, const ShaderType type)
{
    if (type == GL_FRAGMENT_SHADER)
    {
        CompileShaderStage(shaderStageId, GL_FRAGMENT_SHADER, ParseShaderStage("../Assets/Shaders/Fallback.frag").c_str());
    }
}
void DeleteShaderProgram(ShaderProgram shaderProgram) { glDeleteProgram(shaderProgram); }
void DeleteShaderStage(ShaderStageId shaderStage) { glDeleteShader(shaderStage); }

} // namespace ShaderInternal
