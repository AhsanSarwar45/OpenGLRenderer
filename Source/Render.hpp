#include "Texture.hpp"

using Framebuffer = unsigned int;

Framebuffer CreateFramebuffer();

Framebuffer CreateDepthFramebuffer(DepthTexture depthTexture);
Framebuffer CreateDepthFramebuffer(TextureDimensions width, TextureDimensions height);