#pragma once

#include "Aliases.hpp"

struct Quad
{
    VertexArrayObject  vao;
    VertexBufferObject vbo;
};

Quad CreateQuad();
void DeleteQuad(Quad quad);
