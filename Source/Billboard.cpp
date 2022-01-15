#include "Billboard.hpp"

#include <iostream>

#include <glad/glad.h>

#include <stb_image/stb_image.h>
#include <string>

#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

Billboard LoadBillboard(const std::filesystem::path& texturePath)
{
    Billboard billboard;
    billboard.shader = ResourceManager::GetInstance().GetBillboardShader();

    float transparentVertices[] = {// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
                                   0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

                                   0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};

    glGenVertexArrays(1, &billboard.vao);
    glGenBuffers(1, &billboard.vbo);
    glBindVertexArray(billboard.vao);
    glBindBuffer(GL_ARRAY_BUFFER, billboard.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    billboard.texture = LoadTexture(texturePath);

    return billboard;
}
