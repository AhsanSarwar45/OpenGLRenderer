#include "Uniform.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

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

// void ShaderSetFloat2(ShaderProgram shaderProgram, const std::string& uniformName, float x, float y)
// {
//     glUniform2f(glGetUniformLocation(shaderProgram, uniformName.c_str()), x, y);
// }

// void ShaderSetFloat2(ShaderProgram shaderProgram, const std::string& uniformName, const glm::vec2& vec)
// {
//     ShaderSetFloat2(shaderProgram, uniformName.c_str(), vec.x, vec.y);
// }

// void ShaderSetInt3(ShaderProgram shaderProgram, const std::string& name, int x, int y, int z)
// {
//     glUniform3i(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
// }

// void ShaderSetFloat3(ShaderProgram shaderProgram, const std::string& name, float x, float y, float z)
// {
//     glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
// }

// void ShaderSetFloat3(ShaderProgram shaderProgram, const std::string& name, const glm::vec3& vec)
// {
//     glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), vec.x, vec.y, vec.z);
// }

// void ShaderSetInt4(ShaderProgram shaderProgram, const std::string& name, int x, int y, int z, int w)
// {
//     glUniform4i(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
// }

// void ShaderSetFloat4(ShaderProgram shaderProgram, const std::string& name, float x, float y, float z, float w)
// {
//     glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
// }

// void ShaderSetMat4(ShaderProgram shaderProgram, const std::string& name, const glm::mat4& matrix)
// {
//     glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
// }