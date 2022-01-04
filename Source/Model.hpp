#pragma once

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "Material.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Transform.hpp"

struct Model
{
    const char*       name;
    Transform         transform;
    std::vector<Mesh> meshes;
    Material          material;
    ShaderProgram     shaderProgram;
};

Model LoadModelOBJ(const std::filesystem::path& path, ShaderProgram shaderProgram, const std::string& name = "Model",
                   bool flipTexture = false);

void DrawModel(const Model& model);
void DrawModel(const Model& model, const ShaderProgram shader);
// void DrawMode(std::vector<Model>& model, Shader& shader);
