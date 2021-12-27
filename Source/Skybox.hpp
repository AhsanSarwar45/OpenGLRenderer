
#pragma once

#include <string.h>
#include <string>

#include "Shader.hpp"

struct Skybox
{
    Shader Shader;

    unsigned int VBO;
    unsigned int VAO;

    unsigned int TextureID;
};

Skybox LoadSkybox(const std::string& path);

void DeleteSkybox(Skybox& skybox);

void DrawSkybox(Skybox& skybox);