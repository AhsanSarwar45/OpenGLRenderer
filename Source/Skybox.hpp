
#pragma once

#include <filesystem>
#include <string>

#include "Shader.hpp"

struct Skybox
{
    ShaderProgram shaderProgram;

    VertexBufferObject vbo;
    VertexArrayObject  vao;

    TextureId textureId;
};

Skybox LoadSkybox(const std::filesystem::path& directoryPath);

void DeleteSkybox(Skybox& skybox);

void DrawSkybox(const Skybox& skybox);