#pragma once

#include <string>
#include <vector>

#include "Aliases.hpp"
#include "Texture.hpp"

struct DSRenderData;

struct Framebuffer
{
    std::vector<Texture> textures;

    FramebufferObject  fbo;
    RenderbufferObject depthRBO;

    WindowDimension width;
    WindowDimension height;
};

Framebuffer CreateGeometryBuffer(TextureDimension width, TextureDimension height);

FramebufferObject CreateFramebufferObject();
void              DeleteFramebufferObject(FramebufferObject framebuffer);
void              DeleteRenderbufferObject(RenderbufferObject renderbuffer);

Framebuffer CreateHDRFramebuffer(TextureDimension width, TextureDimension height);
Framebuffer CreateGeometryFramebuffer(const std::vector<FramebufferTextureData>& framebufferTextures, TextureDimension width,
                                      TextureDimension height);
void        DeleteGeometryFramebuffer(const Framebuffer& geometryBuffer);

Framebuffer CreateDepthFramebuffer(Texture depthTexture);
Framebuffer CreateDepthFramebuffer(TextureDimension width, TextureDimension height);
Framebuffer CreateDepthArrayFramebuffer(uint16_t numDepthMaps, TextureDimension width, TextureDimension height);
Framebuffer CreateDepthArrayFramebuffer(uint16_t numDepthMaps, TextureDimension resolution);
Framebuffer CreateDepthCubemapFramebuffer(TextureDimension resolution);
Framebuffer CreateDepthCubemapArrayFramebuffer(uint16_t numDepthMaps, TextureDimension resolution);
// TODO: Improve API.
Texture CreateFramebufferTexture(unsigned int index, TextureDimension width, TextureDimension height, unsigned int internalFormat,
                                 const std::string& debugName);

void ResizeFramebuffers(TextureDimension width, TextureDimension height);
