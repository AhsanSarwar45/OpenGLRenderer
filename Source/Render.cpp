#include "Render.hpp"
#include "Texture.hpp"

#include <glad/glad.h>

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
