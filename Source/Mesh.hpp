#pragma once

#include <array>
#include <string>
#include <vector>

#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

struct Mesh
{
    std::vector<Vertex>       Vertices;
    std::vector<unsigned int> Indices;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
};

void InitializeMesh(Mesh& mesh);
void DrawMesh(const Mesh& mesh, Shader& shader, Texture& texture);
