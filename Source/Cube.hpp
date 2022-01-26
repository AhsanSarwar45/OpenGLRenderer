#pragma once

#include "Aliases.hpp"

struct Cube
{
    VertexArray  vao;
    VertexBuffer vbo;
};

Cube CreateCube();
void DeleteCube(Cube quad);