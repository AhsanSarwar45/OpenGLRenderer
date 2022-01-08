#pragma once

#include "Aliases.hpp"
#include <vector>

struct DepthTexture;

struct FramebufferTexture
{
    const char* name;
    TextureId   textureId;
};

struct GeometryFramebuffer
{
    Framebuffer                     id;
    std::vector<FramebufferTexture> textures;
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
TextureId CreateFramebufferTexture(unsigned int index, TextureDimensions width, TextureDimensions height,
                                   unsigned int internalFormat);
