#pragma once

#include <algorithm>
#include <filesystem>

#include <string>
#include <vector>

#include <assimp/scene.h>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Transform.hpp"

struct Model
{
    const char*                            name;
    Transform                              transform;
    std::vector<std::shared_ptr<Mesh>>     meshes;
    std::vector<std::shared_ptr<Material>> materials;
    ShaderProgram                          shaderProgram;
};

std::shared_ptr<Model> LoadModelOBJ(const std::filesystem::path& path, ShaderProgram shaderProgram,
                                    const std::string& name = "Model", bool flipTexture = false);

std::shared_ptr<Model> LoadModel(const std::filesystem::path& path, ShaderProgram shaderProgram,
                                 const std::string& name = "Model", bool flipTexture = false);

void SetMaterial(std::shared_ptr<Model> model, std::shared_ptr<Material> material);

namespace ModelInternal
{
std::shared_ptr<Model> ParseScene(const aiScene* scene, const std::filesystem::path& path, bool flipTexture);
std::shared_ptr<Mesh>  ParseMesh(const aiMesh* mesh);
} // namespace ModelInternal
// void DrawMode(std::vector<Model>& model, Shader& shader);
