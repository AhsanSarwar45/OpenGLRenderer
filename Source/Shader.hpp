#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using Shader = unsigned int;

Shader LoadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const char* name,
                  bool cameraTransform = true);

// use/activate the shader
void UseShader(Shader shader);

// utility uniform functions
void ShaderSetBool(Shader shader, const std::string& name, bool value);
void ShaderSetInt(Shader shader, const std::string& name, int value);
void ShaderSetFloat(Shader shader, const std::string& name, float value);
void ShaderSetInt3(Shader shader, const std::string& name, int x, int y, int z);
void ShaderSetFloat3(Shader shader, const std::string& name, float x, float y, float z);
void ShaderSetFloat3(Shader shader, const std::string& name, const glm::vec3& vec);
void ShaderSetInt4(Shader shader, const std::string& name, int x, int y, int z, int w);
void ShaderSetFloat4(Shader shader, const std::string& name, float x, float y, float z, float w);
void ShaderSetMat4(Shader shader, const std::string& name, const glm::mat4& matrix);

namespace ShaderInternal
{
std::string  ParseShader(const std::string& path);
unsigned int CompileShader(unsigned int type, const std::string& source);
} // namespace ShaderInternal
