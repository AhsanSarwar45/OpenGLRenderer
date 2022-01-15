#pragma once

#include <vector>

#include "Aliases.hpp"
#include "Texture.hpp"

struct DSRenderData;

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

struct DepthFramebuffer
{
    Framebuffer  framebuffer;
    DepthTexture depthTexture;
};

struct FramebufferTextureData
{
    const char*  name;
    unsigned int internalFormat;
};

GeometryFramebuffer CreateBPGeometryBuffer(TextureDimension width, TextureDimension height);
GeometryFramebuffer CreatePBRGeometryBuffer(TextureDimension width, TextureDimension height);

Framebuffer CreateFramebuffer();
void        DeleteFramebuffer(Framebuffer framebuffer);

void DeleteRenderbuffer(Renderbuffer renderbuffer);

GeometryFramebuffer CreateGeometryFramebuffer(const std::vector<FramebufferTextureData>& framebufferTextures, TextureDimension width,
                                              TextureDimension height);
void                DeleteGeometryFramebuffer(const GeometryFramebuffer& geometryBuffer);

DepthFramebuffer CreateDepthFramebuffer(DepthTexture depthTexture);
DepthFramebuffer CreateDepthFramebuffer(TextureDimension width, TextureDimension height);
DepthFramebuffer Create3DDepthFramebuffer(TextureDimension resolution);
// TODO: Improve API.
TextureInternalData CreateFramebufferTexture(unsigned int index, TextureDimension width, TextureDimension height,
                                             unsigned int internalFormat);

void ResizeFramebufferTextures(const std::shared_ptr<DSRenderData> renderData, TextureDimension width, TextureDimension height);
