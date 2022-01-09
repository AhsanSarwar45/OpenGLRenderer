#pragma once

#include <vector>

#include "Aliases.hpp"
#include "Texture.hpp"

struct FramebufferTexture
{
    const char*         name;
    TextureInternalData textureData;
};

struct GeometryFramebuffer
{
    std::vector<FramebufferTexture> textures;

    Framebuffer  id;
    Renderbuffer depthRenderBuffer;

    WindowDimension frameBufferWidth;
    WindowDimension frameBufferHeight;
};

struct FramebufferTextureData
{
    const char*  name;
    unsigned int internalFormat;
};

GeometryFramebuffer CreateBlinnPhongGeometryBuffer(TextureDimensions width, TextureDimensions height);
GeometryFramebuffer CreatePBRGeometryBuffer(TextureDimensions width, TextureDimensions height);

Framebuffer CreateFramebuffer();

GeometryFramebuffer CreateGeometryFramebuffer(const std::vector<FramebufferTextureData>& framebufferTextures,
                                              TextureDimensions width, TextureDimensions height);

Framebuffer CreateDepthFramebuffer(DepthTexture depthTexture);
Framebuffer CreateDepthFramebuffer(TextureDimensions width, TextureDimensions height);
// TODO: Improve API.
TextureInternalData CreateFramebufferTexture(unsigned int index, TextureDimensions width, TextureDimensions height,
                                             unsigned int internalFormat);

void ResizeFramebufferTextures(GeometryFramebuffer* geometryFramebuffer, TextureDimensions width,
                               TextureDimensions height);
