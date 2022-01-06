#include "Texture.hpp"

using Framebuffer = unsigned int;

struct GeometryFrameBuffer
{
    Framebuffer id;
    TextureId   gPosition;
    TextureId   gNormal;
    TextureId   gAlbedoSpec;
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

GeometryFrameBuffer CreateGeometryBuffer(TextureDimensions width, TextureDimensions height);

Framebuffer CreateFramebuffer();

Framebuffer CreateDepthFramebuffer(DepthTexture depthTexture);
Framebuffer CreateDepthFramebuffer(TextureDimensions width, TextureDimensions height);

ScreenQuad CreateScreenQuad();

void RenderScreenQuad(const ScreenQuad& screenQuad);