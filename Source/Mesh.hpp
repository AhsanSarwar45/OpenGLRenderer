#pragma once

#include <array>
#include <string>
#include <vector>

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
};

void InitializeMesh(Mesh& mesh);
void DrawMesh(const Mesh& mesh);
