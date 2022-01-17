
#include "Material.hpp"
#include "Aliases.hpp"

#include <glad/glad.h>

std::shared_ptr<Material> CreateMaterial(ShaderProgram shaderProgram)
{
    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->shaderProgram            = shaderProgram;

    int total = -1;
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &total);
    for (int i = 0; i < total; ++i)
    {
        int    name_len = -1, num = -1;
        GLenum type = GL_ZERO;
        char   name[100];
        glGetActiveUniform(shaderProgram, GLuint(i), sizeof(name) - 1, &name_len, &num, &type, name);
        name[name_len]  = 0;
        GLuint location = glGetUniformLocation(shaderProgram, name);

        switch (type)
        {
        case GL_SAMPLER_2D:
            material->textureUniforms.push_back({.name = name, .textureId = 0, .location = location});
            break;
        default:
            break;
        }

        // if (type == GL_FLOAT_MAT4)

        // else if (type == GL_FLOAT_VEC3)
        //     typeString = "vec3";
        // else if (type == GL_FLOAT_VEC4)
        //     typeString = "vec4";
        // else if (type == GL_FLOAT)
        //     typeString = "float";
        // else if (type == GL_INT)
        //     typeString = "int";
        // else if (type == GL_BOOL)
        //     typeString = "bool";
        // else if (type == GL_SAMPLER_2D)
        //     typeString = "sampler2d";
        // else
        //     typeString = type;
    }

    return material;
}
void SetMaterialTexture(std::shared_ptr<Material> material, const std::string& name, TextureId id)
{
    for (auto& textureUniform : material->textureUniforms)
    {
        if (textureUniform.name == name)
        {
            textureUniform.textureId = id;
            return;
        }
    }
}

void SetMaterialTextureLocation(std::shared_ptr<Material> material, const std::string& name, UniformLocation uniformLocation)
{
    for (auto& textureUniform : material->textureUniforms)
    {
        if (textureUniform.name == name)
        {
            textureUniform.location = uniformLocation;
            return;
        }
    }
}

void SetMaterialShader(std::shared_ptr<Material> material, ShaderProgram shaderProgram)
{
    material->shaderProgram = shaderProgram;

    int total = -1;
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &total);
    for (int i = 0; i < total; ++i)
    {
        int    name_len = -1, num = -1;
        GLenum type = GL_ZERO;
        char   name[100];
        glGetActiveUniform(shaderProgram, GLuint(i), sizeof(name) - 1, &name_len, &num, &type, name);
        name[name_len]  = 0;
        GLuint location = glGetUniformLocation(shaderProgram, name);

        switch (type)
        {
        case GL_SAMPLER_2D:
            SetMaterialTextureLocation(material, name, location);
            break;
        default:
            break;
        }
    }
}
