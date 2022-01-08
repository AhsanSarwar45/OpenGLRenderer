#pragma once

#include <memory>

#include "Aliases.hpp"

#include "Shader.hpp"
#include "Texture.hpp"

struct Scene;
struct Model;
struct Material;
struct Mesh;

struct BlinnPhongGeometryFramebuffer
{
    Framebuffer id;
    TextureId   gPosition;
    TextureId   gNormal;
    TextureId   gAlbedoSpec;
};

struct PBRGeometryFramebuffer
{
    Framebuffer id;
    TextureId   gPosition;
    TextureId   gNormal;
    TextureId   gAlbedo;
    TextureId   gMetalnessRoughnessAO;
};

struct ScreenQuad
{

    unsigned int vao;
    unsigned int vbo;
};

struct DeferredRenderData
{
    WindowDimension frameBufferWidth;
    WindowDimension frameBufferHeight;

    PBRGeometryFramebuffer gBuffer;
    ScreenQuad             screenQuad;

    ShaderProgram pbrGeometryPassShader;
    ShaderProgram pbrLightPassShader;
};

BlinnPhongGeometryFramebuffer CreateBlinnPhongGeometryBuffer(TextureDimensions width, TextureDimensions height);
PBRGeometryFramebuffer        CreatePBRGeometryBuffer(TextureDimensions width, TextureDimensions height);

Framebuffer CreateFramebuffer();

Framebuffer CreateDepthFramebuffer(DepthTexture depthTexture);
Framebuffer CreateDepthFramebuffer(TextureDimensions width, TextureDimensions height);

ScreenQuad CreateScreenQuad();

DeferredRenderData CreateDeferredRenderData(const WindowDimension width, const WindowDimension height);

void RenderGeometryPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data);
void RenderLightPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data, glm::vec3 cameraPos);
void RenderForwardPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data);

void RenderScreenQuad(std::shared_ptr<const ScreenQuad> screenQuad);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, const ShaderProgram shaderProgram,
                const std::shared_ptr<const Material> material);
void RenderModel(const std::shared_ptr<const Model> model, const ShaderProgram shader);
void RenderModel(const std::shared_ptr<const Model> model);
