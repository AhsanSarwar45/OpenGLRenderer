#include "Framebuffer.hpp"

#include <vector>

#include <glad/glad.h>

#include "Render.hpp"
#include "Texture.hpp"

Framebuffer CreateFrameBuffer()
{
    Framebuffer fbo;
    glGenFramebuffers(1, &fbo);

    return fbo;
}

DepthFramebuffer CreateDepthFramebuffer(DepthTexture depthTexture)
{
    Framebuffer depthFramebuffer = CreateFrameBuffer();

    glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);
    glDrawBuffer(GL_NONE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.id, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return {depthFramebuffer, depthTexture};
}

DepthFramebuffer CreateDepthFramebuffer(TextureDimension width, TextureDimension height)
{
    DepthTexture depthTexture = CreateDepthTexture(width, height);
    return CreateDepthFramebuffer(depthTexture);
}

DepthFramebuffer Create3DDepthFramebuffer(TextureDimension resolution)
{
    DepthTexture depthTexture = CreateCubeMapTexture(resolution);
    return CreateDepthFramebuffer(depthTexture);
}

void DeleteFramebuffer(Framebuffer framebuffer) { glDeleteFramebuffers(1, &framebuffer); }

void DeleteRenderbuffer(Renderbuffer renderbuffer) { glDeleteRenderbuffers(1, &renderbuffer); }

GeometryFramebuffer CreateGeometryFramebuffer(const std::vector<FramebufferTextureData>& framebufferTextures, TextureDimension width,
                                              TextureDimension height)
{

    GeometryFramebuffer gBuffer;
    gBuffer.frameBufferWidth  = width;
    gBuffer.frameBufferHeight = height;

    gBuffer.id = CreateFrameBuffer();

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.id);

    int numTextures = framebufferTextures.size();

    gBuffer.textures = std::vector<FramebufferTexture>(numTextures);

    std::vector<unsigned int> attachments = std::vector<unsigned int>(numTextures);

    for (int i = 0; i < numTextures; i++)
    {
        FramebufferTextureData texture = framebufferTextures[i];
        gBuffer.textures[i] = {.name = texture.name, .textureData = CreateFramebufferTexture(i, width, height, texture.internalFormat)};
        attachments[i]      = GL_COLOR_ATTACHMENT0 + i;
    }

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    glDrawBuffers(attachments.size(), &attachments[0]);

    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &gBuffer.depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, gBuffer.depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBuffer.depthRenderBuffer);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer not complete!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return gBuffer;
}

void DeleteGeometryFramebuffer(const GeometryFramebuffer& geometryBuffer)
{
    DeleteFramebuffer(geometryBuffer.frameBufferHeight);
    DeleteRenderbuffer(geometryBuffer.depthRenderBuffer);
    for (const auto& texture : geometryBuffer.textures)
    {
        DeleteTexture(texture.textureData.id);
    }
}

GeometryFramebuffer CreateGeometryBuffer(TextureDimension width, TextureDimension height)
{
    auto framebufferTextures = std::vector<FramebufferTextureData>(4);

    framebufferTextures[0] = {"gPosition", GL_RGBA16F};
    framebufferTextures[1] = {"gNormal", GL_RGBA16F};
    framebufferTextures[2] = {"gAlbedo", GL_RGBA};
    framebufferTextures[3] = {"gMetalnessRoughnessAO", GL_RGBA};

    return CreateGeometryFramebuffer(framebufferTextures, width, height);
}

TextureInternalData CreateFramebufferTexture(unsigned int index, TextureDimension width, TextureDimension height,
                                             unsigned int internalFormat)
{
    TextureInternalData textureData;

    textureData.internalFormat = internalFormat;

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
    else if (internalFormat == GL_RGBA)
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

    return textureData;
}

void ResizeFramebufferTextures(const std::shared_ptr<DSRenderData> renderData, TextureDimension width, TextureDimension height)
{
    GeometryFramebuffer geometryFramebuffer = renderData->gBuffer;

    geometryFramebuffer.frameBufferWidth  = width;
    geometryFramebuffer.frameBufferHeight = height;

    for (const auto& texture : geometryFramebuffer.textures)
    {
        glBindTexture(GL_TEXTURE_2D, texture.textureData.id);
        glTexImage2D(GL_TEXTURE_2D, 0, texture.textureData.internalFormat, width, height, 0, texture.textureData.format,
                     texture.textureData.type, NULL);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, geometryFramebuffer.depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    renderData->gBuffer = geometryFramebuffer;
    // printf("%d\n", renderData->gBuffer.frameBufferWidth);
}