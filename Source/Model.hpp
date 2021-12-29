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
    const char*          name;
    Transform            transform;
    std::vector<Mesh>    meshes;
    std::vector<Texture> textures;
    Shader               shader;
};

Model LoadModelOBJ(const std::filesystem::path& path, Shader shader, const std::string& name = "Model");

void DrawModel(Model& model);
// void DrawMode(std::vector<Model>& model, Shader& shader);
