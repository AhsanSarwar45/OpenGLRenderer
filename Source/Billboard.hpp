#pragma once

#include "Model.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "glm/fwd.hpp"

struct Billboard
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec2 scale    = {1.0f, 1.0f};

    ShaderProgram shader;

    unsigned int vbo;
    unsigned int vao;

    Texture texture;
};

Billboard LoadBillboard(const std::filesystem::path& texturePath);