#pragma once

#include "Aliases.hpp"

struct Quad
{
    VertexArray  vao;
    VertexBuffer vbo;
};

Quad CreateQuad();
void DeleteQuad(Quad quad);
