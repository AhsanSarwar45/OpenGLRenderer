#include "Quad.hpp"

#include <glad/glad.h>

Quad CreateQuad()
{
    Quad quad;

    float vertices[20] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quad.vao);
    glGenBuffers(1, &quad.vbo);
    glBindVertexArray(quad.vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    return quad;
}

void DeleteQuad(Quad quad)
{
    glDeleteVertexArrays(1, &quad.vao);
    glDeleteBuffers(1, &quad.vbo);
}
