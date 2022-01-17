#pragma once

#include <functional>
#include <memory>

#include "Aliases.hpp"
#include "Framebuffer.hpp"
#include "Quad.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

struct Scene;
struct Model;
struct Material;
struct Mesh;
struct Billboard;

using RenderPass = std::function<void(const std::shared_ptr<const Scene>)>;

// Deferred Shading Render Data
struct DSRenderData
{

    GeometryFramebuffer gBuffer;
    Quad                screenQuad;

    ShaderProgram geometryPassShader;
    ShaderProgram lightPassShader;
};

// Forward Shading Render Data
struct ForwardRenderData
{
    ShaderProgram forwardPassShader;
    ShaderProgram depthPassShader;

    DepthFramebuffer depthFramebuffer;
    WindowDimension  width;
    WindowDimension  height;
};

struct RenderingPipeline
{
    const char* name;

    std::vector<RenderPass> renderPasses;
};

DSRenderData CreateDSRenderData(WindowDimension width, WindowDimension height);

ForwardRenderData CreateForwardRenderData(WindowDimension width, WindowDimension height, TextureDimension shadowResolution);

void ResizeForwardViewport(const std::shared_ptr<ForwardRenderData> renderData, TextureDimension width, TextureDimension height);

void DeleteDSRenderData(const std::shared_ptr<const DSRenderData> renderData);

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);

void SetUpLightPassShader(ShaderProgram lightPassShader, const std::vector<FramebufferTexture>& textures);

void RenderTransparentPass(const std::shared_ptr<const Scene> scene);

void RenderForward(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const ForwardRenderData> renderData);
void RenderForwardShadowPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const ForwardRenderData> renderData);

void RenderQuad(const Quad& screenQuad);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, ShaderProgram shaderProgram, const std::shared_ptr<const Material> material);
void RenderModel(const std::shared_ptr<const Model> model, ShaderProgram shader);
void RenderBillboard(const std::shared_ptr<const Billboard> billboard, ShaderProgram shaderProgram, TextureId textureId);
