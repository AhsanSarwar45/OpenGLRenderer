
#pragma once

#include <string.h>
#include <string>

#include "Shader.hpp"

struct Skybox
{
    ShaderProgram shaderProgram;

    unsigned int vbo;
    unsigned int vao;

    unsigned int textureId;
};

Skybox LoadSkybox(const std::string& path);

void DeleteSkybox(Skybox& skybox);

void DrawSkybox(const Skybox& skybox);