#include "Framebuffer.hpp"

#include <glad/glad.h>
#include <vector>

#include "Texture.hpp"

Framebuffer CreateFrameBuffer()
{
    Framebuffer fbo;
    glGenFramebuffers(1, &fbo);

    return fbo;
}

Framebuffer CreateDepthFramebuffer(DepthTexture depthTexture)
{
    Framebuffer fbo;
    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}

Framebuffer CreateDepthFramebuffer(TextureDimensions width, TextureDimensions height)
{
    DepthTexture depthTexture = CreateDepthTexture(width, height);
    return CreateDepthFramebuffer(depthTexture);
}

GeometryFramebuffer CreateGeometryFramebuffer(const std::vector<FramebufferTextureData>& framebufferTextures,
                                              TextureDimensions width, TextureDimensions height)
{
    GeometryFramebuffer gBuffer;
    gBuffer.id = CreateFrameBuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.id);

    int numTextures = framebufferTextures.size();

    gBuffer.textures = std::vector<FramebufferTexture>(numTextures);

    std::vector<unsigned int> attachments = std::vector<unsigned int>(numTextures);

    for (int i = 0; i < numTextures; i++)
    {
        FramebufferTextureData texture = framebufferTextures[i];
        gBuffer.textures[i]            = {.name      = texture.name,
                               .textureId = CreateFramebufferTexture(i, width, height, texture.internalFormat)};
        attachments[i]                 = GL_COLOR_ATTACHMENT0 + i;
    }

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    glDrawBuffers(4, &attachments[0]);

    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer not complete!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return gBuffer;
}

GeometryFramebuffer CreatePBRGeometryBuffer(TextureDimensions width, TextureDimensions height)
{
    auto framebufferTextures = std::vector<FramebufferTextureData>(4);

    framebufferTextures[0] = {"gPosition", GL_RGBA16F};
    framebufferTextures[1] = {"gNormal", GL_RGBA16F};
    framebufferTextures[2] = {"gAlbedo", GL_RGBA};
    framebufferTextures[3] = {"gMetalnessRoughnessAO", GL_RGBA};

    return CreateGeometryFramebuffer(framebufferTextures, width, height);
}

GeometryFramebuffer CreateBlinnPhongGeometryBuffer(TextureDimensions width, TextureDimensions height)
{
    auto framebufferTextures = std::vector<FramebufferTextureData>(3);

    framebufferTextures[0] = {"gPosition", GL_RGBA16F};
    framebufferTextures[1] = {"gNormal", GL_RGBA16F};
    framebufferTextures[2] = {"gAlbedoSpec", GL_RGBA};

    return CreateGeometryFramebuffer(framebufferTextures, width, height);
}

TextureId CreateFramebufferTexture(unsigned int index, TextureDimensions width, TextureDimensions height,
                                   unsigned int internalFormat)
{
    TextureId textureId;

    GLenum format, type;

    if (internalFormat == GL_RGBA16F || internalFormat == GL_RGBA32F)
    {
        format = GL_RGBA;
        type   = GL_FLOAT;
    }
    else if (internalFormat == GL_RGB16F || internalFormat == GL_RGB32F)
    {
        format = GL_RGB;
        type   = GL_FLOAT;
    }
    else if (internalFormat == GL_RGBA)
    {
        format = GL_RGBA;
        type   = GL_UNSIGNED_BYTE;
    }
    else if (internalFormat == GL_RGB)
    {
        format = GL_RGB;
        type   = GL_UNSIGNED_BYTE;
    }
    else
    {
        printf("Framebuffer texture type not supported!\n");
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, textureId, 0);

    return textureId;
}
