#include "Framebuffer.hpp"

#include <vector>

#include <glad/glad.h>

#include "Render.hpp"
#include "ResourceManager.hpp"

#include "Texture.hpp"

FramebufferObject CreateFramebufferObject()
{
    FramebufferObject fbo;
    glGenFramebuffers(1, &fbo);

    return fbo;
}

Framebuffer CreateDepthFramebuffer(Texture depthTexture)
{
    FramebufferObject fbo = CreateFramebufferObject();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.id, 0);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return {
        .textures = {depthTexture},
        .fbo      = fbo,
        .width    = depthTexture.width,
        .height   = depthTexture.height,
    };
}

Framebuffer CreateDepthFramebuffer(TextureDimension width, TextureDimension height)
{
    Texture depthTexture = CreateDepthTexture(width, height);
    return CreateDepthFramebuffer(depthTexture);
}

Framebuffer CreateDepthArrayFramebuffer(uint16_t numDepthMaps, TextureDimension width, TextureDimension height)
{
    Texture depthTexture = CreateDepthTextureArray(numDepthMaps, width, height);
    return CreateDepthFramebuffer(depthTexture);
}

Framebuffer CreateDepthArrayFramebuffer(uint16_t numDepthMaps, TextureDimension resolution)
{
    return CreateDepthArrayFramebuffer(numDepthMaps, resolution, resolution);
}

Framebuffer CreateDepthCubemapArrayFramebuffer(uint16_t numDepthMaps, TextureDimension resolution)
{
    Texture depthTexture = CreateDepthCubemapArray(numDepthMaps, resolution);
    return CreateDepthFramebuffer(depthTexture);
}

Framebuffer CreateDepthCubemapFramebuffer(TextureDimension resolution)
{
    Texture depthTexture = CreateDepthCubemap(resolution);
    return CreateDepthFramebuffer(depthTexture);
}

void DeleteFramebufferObject(FramebufferObject fbo) { glDeleteFramebuffers(1, &fbo); }

void DeleteRenderbufferObject(RenderbufferObject rbo) { glDeleteRenderbuffers(1, &rbo); }

Framebuffer CreateHDRFramebuffer(TextureDimension width, TextureDimension height)
{
    Framebuffer hdrFramebuffer;
    hdrFramebuffer.fbo = CreateFramebufferObject();

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFramebuffer.fbo);

    hdrFramebuffer.textures = {CreateFramebufferTexture(0, width, height, GL_RGBA16F, "HDR Texture")};

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer not complete!\n");
    }

    GLenum attachments[] = {GL_COLOR_ATTACHMENT0};

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    glDrawBuffers(1, attachments);

    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &hdrFramebuffer.depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, hdrFramebuffer.depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdrFramebuffer.depthRBO);

    return hdrFramebuffer;
}

Framebuffer CreateGeometryFramebuffer(const std::vector<FramebufferTextureData>& framebufferTextures, TextureDimension width,
                                      TextureDimension height)
{

    Framebuffer gBuffer;
    gBuffer.width  = width;
    gBuffer.height = height;

    gBuffer.fbo = CreateFramebufferObject();

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.fbo);

    int numTextures = framebufferTextures.size();

    gBuffer.textures = std::vector<Texture>(numTextures);

    std::vector<unsigned int> attachments = std::vector<unsigned int>(numTextures);

    for (int i = 0; i < numTextures; i++)
    {
        FramebufferTextureData texture = framebufferTextures[i];
        gBuffer.textures[i]            = CreateFramebufferTexture(i, width, height, texture.internalFormat, texture.name);
        attachments[i]                 = GL_COLOR_ATTACHMENT0 + i;
    }

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    glDrawBuffers(attachments.size(), &attachments[0]);

    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &gBuffer.depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gBuffer.depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBuffer.depthRBO);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer not complete!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return gBuffer;
}

void DeleteGeometryFramebuffer(const Framebuffer& geometryBuffer)
{
    DeleteFramebufferObject(geometryBuffer.fbo);
    DeleteRenderbufferObject(geometryBuffer.depthRBO);
    for (const auto& texture : geometryBuffer.textures)
    {
        DeleteTexture(texture.id);
    }
}

Framebuffer CreateGeometryBuffer(TextureDimension width, TextureDimension height)
{
    std::vector<FramebufferTextureData> framebufferTextures = {
        {"gPositionMetalness", GL_RGBA16F},
        {"gNormalRoughness", GL_RGBA16F},
        {"gAlbedoAO", GL_RGBA8},
    };

    return CreateGeometryFramebuffer(framebufferTextures, width, height);
}

Texture CreateFramebufferTexture(unsigned int index, TextureDimension width, TextureDimension height, unsigned int internalFormat,
                                 const std::string& debugName)
{
    Texture textureData = {.internalFormat = internalFormat, .width = width, .height = height, .debugName = debugName};

    if (internalFormat == GL_RGBA16F || internalFormat == GL_RGBA32F)
    {
        textureData.format = GL_RGBA;
        textureData.type   = GL_FLOAT;
    }
    else if (internalFormat == GL_RGB16F || internalFormat == GL_RGB32F)
    {
        textureData.format = GL_RGB;
        textureData.type   = GL_FLOAT;
    }
    else if (internalFormat == GL_RGBA || internalFormat == GL_RGBA8)
    {
        textureData.format = GL_RGBA;
        textureData.type   = GL_UNSIGNED_BYTE;
    }
    else if (internalFormat == GL_RGB)
    {
        textureData.format = GL_RGB;
        textureData.type   = GL_UNSIGNED_BYTE;
    }
    else
    {
        printf("Framebuffer texture type not supported!\n");
    }

    textureData.id = CreateTexture();
    glBindTexture(GL_TEXTURE_2D, textureData.id);
    glTexImage2D(GL_TEXTURE_2D, 0, textureData.internalFormat, width, height, 0, textureData.format, textureData.type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, textureData.id, 0);

    glObjectLabel(GL_TEXTURE, textureData.id, strlen(debugName.c_str()), debugName.c_str());

    return textureData;
}

void ResizeFramebuffers(TextureDimension width, TextureDimension height)
{
    std::vector<Framebuffer*> framebuffersToResize = ResourceManager::GetInstance().GetFramebuffersToResize();
    for (auto&& framebuffer : framebuffersToResize)
    {
        framebuffer->width  = width;
        framebuffer->height = height;

        for (const auto& texture : framebuffer->textures)
        {
            glBindTexture(GL_TEXTURE_2D, texture.id);
            glTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat, width, height, 0, texture.format, texture.type, NULL);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    }
}