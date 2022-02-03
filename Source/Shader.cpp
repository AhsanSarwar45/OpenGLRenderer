#include "Shader.hpp"

#include <fstream>

#include <functional>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>

#include <glad/glad.h>

#include <vector>

#include "ResourceManager.hpp"
#include "Utilities/Hash.hpp"

using namespace ShaderInternal;

ShaderProgram LoadShader(const std::vector<std::filesystem::path>& shaderStagePaths, const char* name, bool cameraTransform,
                         std::function<void(ShaderProgram)> initFunction)
{
    std::vector<ShaderStage> shaderStages;
    for (const auto& shaderStagePath : shaderStagePaths)
    {
        std::filesystem::path path = ResourceManager::GetInstance().GetRootPath() / shaderStagePath;

        GLenum shaderType;

        std::string ext = path.extension().string();

        if (ext == ".vert") // TODO check for multiplt file name conventions
            shaderType = GL_VERTEX_SHADER;
        else if (ext == ".frag")
            shaderType = GL_FRAGMENT_SHADER;
        else if (ext == ".geom")
            shaderType = GL_GEOMETRY_SHADER;
        else if (ext == ".tesc")
            shaderType = GL_TESS_CONTROL_SHADER;
        else if (ext == ".tese")
            shaderType = GL_TESS_EVALUATION_SHADER;
        else if (ext == ".comp")
            shaderType = GL_COMPUTE_SHADER;
        else
            return 999; // TODO return fallback shader

        shaderStages.push_back({.path = path, .type = shaderType});
    }

    ShaderProgram shaderProgram = glCreateProgram();

    LoadShaderStages(shaderProgram, shaderStages);

    if (initFunction)
    {
        initFunction(shaderProgram);
    }

    for (auto& shaderData : shaderStages)
    {
        shaderData.path.make_preferred();

        ResourceManager::GetInstance().AddShader(shaderProgram, shaderData.path, shaderStages, initFunction);
    }

    glObjectLabel(GL_PROGRAM, shaderProgram, strlen(name), name);

    return shaderProgram;
}

// ShaderProgram LoadShader(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath, const char* name,
//                          bool cameraTransform)
// {
//     return LoadShaders({vertexShaderPath.string(), fragmentShaderPath.string()}, name, cameraTransform);
// }

void UseShaderProgram(const ShaderProgram shaderProgram) { glUseProgram(shaderProgram); }

namespace ShaderInternal
{
void LoadShaderStages(ShaderProgram shaderProgram, std::vector<ShaderStage>& shaderStages)
{
    bool vertexCompilationFailed = false;
    // find references to existing shaders, and create ones that didn't exist previously.
    for (auto& shaderData : shaderStages)
    {
        ShaderStageId shaderStageId = CreateShaderStage(shaderData.type);
        if (!vertexCompilationFailed)
        {
            if (!CompileShaderStage(shaderStageId, shaderData.type, ParseShaderStage(shaderData.path).c_str()))
            {
                vertexCompilationFailed = shaderData.type == GL_VERTEX_SHADER;
            }
        }
        else
        {
            SetToFallback(shaderStageId, shaderData.type);
        }

        shaderData.id = shaderStageId;

        glAttachShader(shaderProgram, shaderStageId);
    }

    LinkAndValidateProgram(shaderProgram, shaderStages);
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

bool LinkAndValidateProgram(const ShaderProgram shaderProgram, const std::vector<ShaderStage>& shaderStages)
{
    bool linkSuccess = LinkProgram(shaderProgram);

    DeleteShaderStages(shaderProgram, shaderStages);

    if (linkSuccess)
    {
        bool validateSuccess = ValidateProgram(shaderProgram);
        if (linkSuccess)
        {
            return true;
        }
    }

    return false;
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

bool CompileShaderStage(const ShaderStageId shaderStageId, ShaderType type, std::string source, bool needFallback)
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
        if (needFallback)
        {
            SetToFallback(shaderStageId, type);
        }

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

    std::cout << "Shader Source (Shader Stage ID: " << shaderStage << "): " << source;
}

void SetToFallback(const ShaderStage shaderStage) { SetToFallback(shaderStage.id, shaderStage.type); }

void SetToFallback(const ShaderStageId shaderStageId, const ShaderType type)
{
    if (type == GL_VERTEX_SHADER)
    {

        CompileShaderStage(shaderStageId, GL_VERTEX_SHADER, ParseShaderStage("../Assets/Shaders/Fallback/Fallback.vert").c_str(), false);
    }
    if (type == GL_FRAGMENT_SHADER)
    {
        CompileShaderStage(shaderStageId, GL_FRAGMENT_SHADER, ParseShaderStage("../Assets/Shaders/Fallback/Fallback.frag").c_str(), false);
    }
    if (type == GL_GEOMETRY_SHADER)
    {
        CompileShaderStage(shaderStageId, GL_GEOMETRY_SHADER, ParseShaderStage("../Assets/Shaders/Fallback/Fallback.geom").c_str(), false);
    }
}
void DeleteShaderProgram(ShaderProgram shaderProgram) { glDeleteProgram(shaderProgram); }

void DeleteShaderStages(ShaderProgram shaderProgram, const std::vector<ShaderStage>& shaderStages)
{

    for (auto& shaderData : shaderStages)
    {
        glDetachShader(shaderProgram, shaderData.id);
        DeleteShaderStage(shaderData.id);
    }
}

void DeleteShaderStage(ShaderStageId shaderStage) { glDeleteShader(shaderStage); }

} // namespace ShaderInternal
