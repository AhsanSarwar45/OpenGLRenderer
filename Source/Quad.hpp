#pragma once

struct Quad
{
    unsigned int vao;
    unsigned int vbo;
};

Quad CreateQuad();
void DeleteQuad(Quad quad);
