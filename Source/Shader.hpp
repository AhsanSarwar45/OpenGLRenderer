#pragma once

#include <filesystem>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// TODO mind ur naming!!

using ShaderProgram = unsigned int;

ShaderProgram LoadShader(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath,
                         const char* shaderProgramName, bool cameraTransform = true);
ShaderProgram LoadShaders(const std::vector<std::string>& shaderStagePaths, const char* shaderProgramName,
                          bool cameraTransform = true);

using ShaderStageId = unsigned int;
using ShaderType    = unsigned int; // TODO rename this

struct ShaderStage
{
    std::filesystem::path path;
    ShaderType            type; // todo maybe dont need this
    ShaderStageId         id;
};

// use/activate the shader
void UseShaderProgram(const ShaderProgram shaderProgram);

// utility uniform functions
void ShaderSetBool(ShaderProgram shaderProgram, const std::string& uniformName, bool value);
void ShaderSetInt(ShaderProgram shaderProgram, const std::string& uniformName, int value);
void ShaderSetFloat(ShaderProgram shaderProgram, const std::string& uniformName, float value);
void ShaderSetInt3(ShaderProgram shaderProgram, const std::string& uniformName, int x, int y, int z);
void ShaderSetFloat3(ShaderProgram shaderProgram, const std::string& uniformName, float x, float y, float z);
void ShaderSetFloat3(ShaderProgram shaderProgram, const std::string& uniformName, const glm::vec3& vec);
void ShaderSetInt4(ShaderProgram shaderProgram, const std::string& uniformName, int x, int y, int z, int w);
void ShaderSetFloat4(ShaderProgram shaderProgram, const std::string& uniformName, float x, float y, float z, float w);
void ShaderSetMat4(ShaderProgram shaderProgram, const std::string& uniformName, const glm::mat4& matrix);

namespace ShaderInternal
{

ShaderProgram LoadShaderStages(std::vector<struct ShaderStage>& shaderStages, const char* name,
                               bool cameraTransform = true);
std::string   ParseShaderStage(const std::filesystem::path& path);
bool          ReloadShaderStage(const ShaderProgram shaderProgram, ShaderStage shaderStage);
bool          CompileShaderStage(const ShaderStageId shaderStageId, std::string source);
bool          LinkProgram(const ShaderProgram shaderProgram);
bool          ValidateProgram(const ShaderProgram shaderProgram);
void          PrintShaderStageSource(const ShaderStageId shaderStageId);
ShaderStageId CreateShaderStage(const ShaderType type);
} // namespace ShaderInternal
