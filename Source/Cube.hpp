#pragma once

#include "Aliases.hpp"

struct Cube
{
    VertexArrayObject  vao;
    VertexBufferObject vbo;
};

Cube CreateCube();
void DeleteCube(Cube quad);