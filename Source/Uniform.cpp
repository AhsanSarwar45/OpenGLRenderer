#include "Uniform.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

UniformBufferObject CreateUniformBufferObject(UniformBufferBinding binding, size_t size)
{
    UniformBufferObject uniformBufferObject;
    glGenBuffers(1, &uniformBufferObject);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, uniformBufferObject);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
    return uniformBufferObject;
}

void SetUniformBufferSubData(UniformBufferObject uniformBufferObject, void* data, size_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferObject);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
}

void ShaderSetBool(ShaderProgram shaderProgram, const std::string& uniformName, bool value)
{
    ShaderSetBool(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), value);
}
void ShaderSetBool(ShaderProgram shaderProgram, UniformLocation uniformlocation, bool value) { glUniform1i(uniformlocation, (int)value); }

void ShaderSetInt(ShaderProgram shaderProgram, const std::string& uniformName, int value)
{
    ShaderSetInt(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), value);
}
void ShaderSetInt(ShaderProgram shaderProgram, UniformLocation uniformlocation, int value) { glUniform1i(uniformlocation, value); }

void ShaderSetFloat(ShaderProgram shaderProgram, const std::string& uniformName, float value)
{
    ShaderSetFloat(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), value);
}
void ShaderSetFloat(ShaderProgram shaderProgram, UniformLocation uniformlocation, float value) { glUniform1f(uniformlocation, value); }

void ShaderSetInt2(ShaderProgram shaderProgram, const std::string& uniformName, const glm::ivec2& vec)
{
    ShaderSetInt2(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), vec);
}

void ShaderSetInt2(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::ivec2& vec)
{
    glUniform2i(uniformlocation, vec.x, vec.y);
}

void ShaderSetFloat2(ShaderProgram shaderProgram, const std::string& uniformName, const glm::vec2& vec)
{
    ShaderSetFloat2(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), vec);
}

void ShaderSetFloat2(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::vec2& vec)
{
    glUniform2f(uniformlocation, vec.x, vec.y);
}

void ShaderSetInt3(ShaderProgram shaderProgram, const std::string& uniformName, const glm::ivec3& vec)
{
    ShaderSetInt3(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), vec);
}

void ShaderSetInt3(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::ivec3& vec)
{
    glUniform3i(uniformlocation, vec.x, vec.y, vec.z);
}

void ShaderSetFloat3(ShaderProgram shaderProgram, const std::string& uniformName, const glm::vec3& vec)
{
    ShaderSetFloat3(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), vec);
}

void ShaderSetFloat3(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::vec3& vec)
{
    glUniform3f(uniformlocation, vec.x, vec.y, vec.z);
}

void ShaderSetInt4(ShaderProgram shaderProgram, const std::string& uniformName, const glm::ivec4& vec)
{
    ShaderSetInt4(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), vec);
}

void ShaderSetInt4(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::ivec4& vec)
{
    glUniform4i(uniformlocation, vec.x, vec.y, vec.z, vec.w);
}

void ShaderSetFloat4(ShaderProgram shaderProgram, const std::string& uniformName, const glm::vec4& vec)
{
    ShaderSetFloat4(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), vec);
}

void ShaderSetFloat4(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::vec4& vec)
{
    glUniform4f(uniformlocation, vec.x, vec.y, vec.z, vec.w);
}

void ShaderSetMat4(ShaderProgram shaderProgram, const std::string& uniformName, const glm::mat4& matrix)
{
    ShaderSetMat4(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), matrix);
}

void ShaderSetMat4(ShaderProgram shaderProgram, UniformLocation uniformlocation, const glm::mat4& matrix)
{
    glUniformMatrix4fv(uniformlocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderSetFloatArray(ShaderProgram shaderProgram, UniformLocation uniformlocation, size_t length, const float* array)
{
    glUniform1fv(uniformlocation, length, array);
}

void ShaderSetFloatArray(ShaderProgram shaderProgram, const std::string& uniformName, size_t length, const float* array)
{
    ShaderSetFloatArray(shaderProgram, glGetUniformLocation(shaderProgram, uniformName.c_str()), length, array);
}
