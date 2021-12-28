#pragma once

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Transform.hpp"

struct Model
{
    std::string          name;
    Transform            transform;
    std::vector<Mesh>    meshes;
    std::vector<Texture> textures;
};

Model LoadModelOBJ(const std::filesystem::path& path, const std::string& name = "Model");

void DrawModel(Model& model, Shader& shader);
// void DrawMode(std::vector<Model>& model, Shader& shader);
