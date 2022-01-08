#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "Material.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

struct Mesh
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    int materialId;
};

void InitializeMesh(std::shared_ptr<Mesh> mesh);
