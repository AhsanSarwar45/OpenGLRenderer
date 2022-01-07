#pragma once

#include "Mesh.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <memory>

using Framebuffer = unsigned int;

struct BlinnPhongGeometryFramebuffer
{
    Framebuffer id;
    TextureId   gPosition;
    TextureId   gNormal;
    TextureId   gAlbedoSpec;
};

struct PBRGeometryFramebuffer
{
    Framebuffer id;
    TextureId   gPosition;
    TextureId   gNormal;
    TextureId   gAlbedo;
    TextureId   gMetalnessRoughnessAO;
};

struct ScreenQuad
{
    float vertices[20] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    unsigned int vao;
    unsigned int vbo;
};

BlinnPhongGeometryFramebuffer CreateBlinnPhongGeometryBuffer(TextureDimensions width, TextureDimensions height);
PBRGeometryFramebuffer        CreatePBRGeometryBuffer(TextureDimensions width, TextureDimensions height);

Framebuffer CreateFramebuffer();

Framebuffer CreateDepthFramebuffer(DepthTexture depthTexture);
Framebuffer CreateDepthFramebuffer(TextureDimensions width, TextureDimensions height);

ScreenQuad CreateScreenQuad();

void RenderScreenQuad(const ScreenQuad& screenQuad);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, const ShaderProgram shaderProgram,
                const std::shared_ptr<const Material> material);
void RenderModel(const std::shared_ptr<const Model> model, const ShaderProgram shader);
void RenderModel(const std::shared_ptr<const Model> model);