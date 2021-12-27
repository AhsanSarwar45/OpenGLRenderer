#include "Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Shader LoadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const char* name,
                  bool cameraTransform)
{
    unsigned int id = glCreateProgram();

    unsigned int vertexShader =
        ShaderInternal::CompileShader(GL_VERTEX_SHADER, ShaderInternal::ParseShader(vertexShaderPath).c_str());
    unsigned int fragmentShader =
        ShaderInternal::CompileShader(GL_FRAGMENT_SHADER, ShaderInternal::ParseShader(fragmentShaderPath).c_str());

    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glValidateProgram(id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (cameraTransform)
    {
        glUniformBlockBinding(id, glGetUniformBlockIndex(id, "Camera"), 0);
    }

    glObjectLabel(GL_PROGRAM, id, strlen(name), name);

    return id;
}

namespace ShaderInternal
{
std::string ParseShader(const std::string& path)
{
    std::ifstream stream(path);
    if (!stream.is_open())
    {
        std::cout << "Failed to open file" << path << "\n";
    }

    std::string       line;
    std::stringstream ss;

    while (getline(stream, line))
    {
        ss << line << "\n";
    }

    return ss.str();
}

unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id  = glCreateShader(type);
    const char*  src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader\n";
        std::cout << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}
} // namespace ShaderInternal

void ShaderSetBool(Shader shader, const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(shader, name.c_str()), (int)value);
}
void ShaderSetInt(Shader shader, const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(shader, name.c_str()), value);
}
void ShaderSetFloat(Shader shader, const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(shader, name.c_str()), value);
}

void ShaderSetInt4(Shader shader, const std::string& name, int x, int y, int z, int w)
{
    glUniform4i(glGetUniformLocation(shader, name.c_str()), x, y, z, w);
}
void ShaderSetFloat4(Shader shader, const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(shader, name.c_str()), x, y, z, w);
}
void ShaderSetInt3(Shader shader, const std::string& name, int x, int y, int z)
{
    glUniform3i(glGetUniformLocation(shader, name.c_str()), x, y, z);
}
void ShaderSetFloat3(Shader shader, const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(shader, name.c_str()), x, y, z);
}
void ShaderSetFloat3(Shader shader, const std::string& name, const glm::vec3& vec)
{
    glUniform3f(glGetUniformLocation(shader, name.c_str()), vec.x, vec.y, vec.z);
}
void ShaderSetMat4(Shader shader, const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void UseShader(Shader shader) { glUseProgram(shader); }
