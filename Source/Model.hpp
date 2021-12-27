#pragma once

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "glm/fwd.hpp"

struct Transform
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale    = {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
};

struct Model
{
    std::string          name;
    Transform            transform;
    std::vector<Mesh>    meshes;
    std::vector<Texture> textures;
};

Model LoadModelOBJ(const std::filesystem::path& path, const std::string& name = "Model");

void DrawModel(Model& model, Shader& shader);
