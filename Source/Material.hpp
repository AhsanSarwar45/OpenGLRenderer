#pragma once

#include <memory>
#include <unordered_map>

#include "Aliases.hpp"
#include "Uniform.hpp"

struct Material
{
    std::vector<TextureUniform> textureUniforms;
    ShaderProgram               shaderProgram;
};

std::shared_ptr<Material> CreateMaterial(ShaderProgram shaderProgram);

void SetMaterialTexture(std::shared_ptr<Material> material, const std::string& name, TextureId id);
void SetMaterialTextureLocation(std::shared_ptr<Material> material, const std::string& name, UniformLocation uniformLocation);
void SetMaterialShader(std::shared_ptr<Material> material, ShaderProgram shaderProgram);
