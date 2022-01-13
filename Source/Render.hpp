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
    DepthTexture  shadowTexture;
};

struct RenderingPipeline
{
    const char* name;

    std::vector<RenderPass> renderPasses;
};

DSRenderData CreatePBRDSRenderData(WindowDimension width, WindowDimension height);
DSRenderData CreateBPDSRenderData(WindowDimension width, WindowDimension height);

ForwardRenderData CreatePBRForwardRenderData();
ForwardRenderData CreateBPForwardRenderData();

void DeleteDSRenderData(const std::shared_ptr<const DSRenderData> renderData);

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);

void SetUpLightPassShader(ShaderProgram lightPassShader, const std::vector<FramebufferTexture>& textures);

void RenderForward(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const ForwardRenderData> renderData);
void RenderForwardShadowPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const ForwardRenderData> renderData);

void RenderQuad(const Quad& screenQuad);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, ShaderProgram shaderProgram, const std::shared_ptr<const Material> material);
void RenderModel(const std::shared_ptr<const Model> model, ShaderProgram shader);
void RenderModel(const std::shared_ptr<const Model> model);
