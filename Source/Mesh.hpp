#pragma once

#include <array>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include "Aliases.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

enum class MeshType
{
    Static,
    Dynamic
};

struct MeshData
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
};

struct Mesh
{
    VertexArrayObject   vao;
    VertexBufferObject  vbo;
    ElementBufferObject ebo;

    MaterialId   materialId;
    unsigned int numIndices;
};

void InitializeMesh(std::shared_ptr<Mesh> mesh, const MeshData& data, MeshType meshType = MeshType::Static);
