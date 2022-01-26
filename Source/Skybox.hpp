
#pragma once

#include <filesystem>
#include <string>

#include "Shader.hpp"

struct Skybox
{
    ShaderProgram shaderProgram;

    unsigned int vbo;
    unsigned int vao;

    unsigned int textureId;
};

Skybox LoadSkybox(const std::filesystem::path& directoryPath);

void DeleteSkybox(Skybox& skybox);

void DrawSkybox(const Skybox& skybox);