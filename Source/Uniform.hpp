#pragma once

#include <corecrt.h>
#include <glm/glm.hpp>

#include "Aliases.hpp"
#include "Texture.hpp"

struct TextureUniform
{
    std::string     name;
    TextureId       textureId;
    UniformLocation location;
};

template <typename T>
struct UniformBufferVector
{
    std::vector<T>      data;
    UniformBufferObject ubo;
    size_t              size;
};

template <typename T>
struct UniformBuffer
{
    T                   data;
    UniformBufferObject ubo;
    size_t              size;
};

UniformBufferObject CreateUniformBufferObject(UniformBufferBinding binding, size_t size);

template <typename T>
UniformBufferVector<T> CreateUniformBufferVector(UniformBufferBinding binding, size_t count)
{
    return {
        .data = std::vector<T>(count),
        .ubo  = CreateUniformBufferObject(binding, count * sizeof(T)),
        .size = count * sizeof(T),
    };
}

template <typename T>
UniformBuffer<T> CreateUniformBuffer(UniformBufferBinding binding)
{
    return {
        .ubo  = CreateUniformBufferObject(binding, sizeof(T)),
        .size = sizeof(T),
    };
}

template <typename T>
void UploadUniformBufferVector(UniformBufferVector<T>& uboVector)
{
    SetUniformBufferSubData(uboVector.ubo, &uboVector.data[0], uboVector.size);
}

template <typename T>
void UploadUniformBuffer(UniformBuffer<T>& uniformBuffer)
{
    SetUniformBufferSubData(uniformBuffer.ubo, &uniformBuffer.data, uniformBuffer.size);
}

void SetUniformBufferSubData(UniformBufferObject uniformBufferObject, void* data, size_t size);

// utility uniform functions
void ShaderSetBool(ShaderProgram shaderProgram, const std::string& uniformName, bool value);
void ShaderSetBool(ShaderProgram shaderProgram, UniformLocation unifromLocation, bool value);

void ShaderSetInt(ShaderProgram shaderProgram, const std::string& uniformName, int value);
void ShaderSetInt(ShaderProgram shaderProgram, UniformLocation uniformLocation, int value);

void ShaderSetFloat(ShaderProgram shaderProgram, const std::string& uniformName, float value);
void ShaderSetFloat(ShaderProgram shaderProgram, UniformLocation uniformlocation, float value);

void ShaderSetInt2(ShaderProgram shaderProgram, const std::string& uniformName, const glm::ivec2& vec);
void ShaderSetInt2(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::ivec2& vec);

void ShaderSetFloat2(ShaderProgram shaderProgram, const std::string& uniformName, const glm::vec2& vec);
void ShaderSetFloat2(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::vec2& vec);

void ShaderSetInt3(ShaderProgram shaderProgram, const std::string& uniformName, const glm::ivec3& vec);
void ShaderSetInt3(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::ivec3& vec);

void ShaderSetFloat3(ShaderProgram shaderProgram, const std::string& uniformName, const glm::vec3& vec);
void ShaderSetFloat3(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::vec3& vec);

void ShaderSetInt4(ShaderProgram shaderProgram, const std::string& uniformName, const glm::ivec4& vec);
void ShaderSetInt4(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::ivec4& vec);

void ShaderSetFloat4(ShaderProgram shaderProgram, const std::string& uniformName, const glm::vec4& vec);
void ShaderSetFloat4(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::vec4& vec);

void ShaderSetMat4(ShaderProgram shaderProgram, const std::string& uniformName, const glm::mat4& matrix);
void ShaderSetMat4(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::mat4& matrix);

void ShaderSetFloatArray(ShaderProgram shaderProgram, const std::string& uniformName, size_t length, const float* array);
void ShaderSetFloatArray(ShaderProgram shaderProgram, UniformLocation uniformlocation, size_t length, const float* array);