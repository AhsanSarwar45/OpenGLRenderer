#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Aliases.hpp"

ShaderProgram LoadShader(const std::vector<std::filesystem::path>& shaderStagePaths, const char* shaderProgramName,
                         bool                               cameraTransform = true,
                         std::function<void(ShaderProgram)> initFunction    = std::function<void(ShaderProgram)>());

struct ShaderStage
{
    std::filesystem::path path;
    ShaderType            type;
    ShaderStageId         id;
};

// use/activate the shader
void UseShaderProgram(const ShaderProgram shaderProgram);

namespace ShaderInternal
{

void          LoadShaderStages(ShaderProgram shaderProgram, std::vector<ShaderStage>& shaderStages);
ShaderStageId LoadShaderStage(const std::filesystem::path& path, ShaderProgram shaderProgram, const ShaderType type);
std::string   ParseShaderStage(const std::filesystem::path& path);
bool          CompileShaderStage(const ShaderStageId shaderStageId, ShaderType type, std::string source, bool needFallback = true);
bool          LinkProgram(const ShaderProgram shaderProgram);
bool          ValidateProgram(const ShaderProgram shaderProgram);
bool          LinkAndValidateProgram(const ShaderProgram shaderProgram, const std::vector<ShaderStage>& shaderStages);
void          PrintShaderStageSource(const ShaderStageId shaderStageId);
void          SetToFallback(const ShaderStage shaderStage);
void          SetToFallback(const ShaderStageId shaderStageId, const ShaderType type);
ShaderStageId CreateShaderStage(const ShaderType type);

void DeleteShaderProgram(ShaderProgram shaderProgram);
void DeleteShaderStages(ShaderProgram shaderProgram, const std::vector<ShaderStage>& shaderStages);
void DeleteShaderStage(ShaderStageId shaderStage);
} // namespace ShaderInternal
