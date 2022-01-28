#pragma once

#include <functional>
#include <memory>
#include <stdint.h>

#include "Aliases.hpp"
#include "Framebuffer.hpp"
#include "Light.hpp"
#include "Quad.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

struct Scene;
struct Model;
struct Material;
struct Mesh;
struct Billboard;

using RenderPass = std::function<void(const std::shared_ptr<const Scene>)>;

struct LightRenderData
{
    uint16_t                    maxLightCount;
    std::vector<LightTransform> lightTransforms;

    UniformBuffer lightTransformUB;
};

struct ShadowRenderData
{
    ShaderProgram    shadowPassShader;
    DepthFramebuffer shadowFramebuffer;
};

// Deferred Shading Render Data
struct DSRenderData
{

    GeometryFramebuffer gBuffer;

    Quad screenQuad;

    ShaderProgram geometryPassShader;
    ShaderProgram lightPassShader;
};

// Forward Shading Render Data
struct ForwardRenderData
{
    ShaderProgram forwardPassShader;

    WindowDimension width;
    WindowDimension height;
};

struct RenderingPipeline
{
    const char* name;
};

DSRenderData CreateDSRenderData(WindowDimension width, WindowDimension height);

ForwardRenderData CreateForwardRenderData(WindowDimension width, WindowDimension height);

LightRenderData  CreateLightRenderData(uint16_t maxLightCount = 32);
ShadowRenderData CreateShadowRenderData(const LightRenderData& lightRenderData, TextureDimension shadowResolution = 1024);

void ResizeForwardViewport(ForwardRenderData* renderData, TextureDimension width, TextureDimension height);

void DeleteDSRenderData(const DSRenderData& renderData);

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const DSRenderData& renderData);
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const DSRenderData& renderData, const ShadowRenderData& shadowRenderData);
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const DSRenderData& renderData);

void SetUpLightPassShader(ShaderProgram lightPassShader, const std::vector<FramebufferTexture>& textures);

void RenderTransparentPass(const std::shared_ptr<const Scene> scene);

void RenderForward(const std::shared_ptr<const Scene> scene, const ForwardRenderData& renderData, const ShadowRenderData& shadowRenderData);

void RenderShadowPass(const std::shared_ptr<const Scene> scene, LightRenderData& lightRenderData, const ShadowRenderData& shadowRenderData);
void RenderForwardShadowPass(const std::shared_ptr<const Scene> scene, const ForwardRenderData& renderData);
void RenderDeferredShadowPass(const std::shared_ptr<const Scene> scene, const DSRenderData& renderData);

void RenderQuad(const Quad& screenQuad);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, ShaderProgram shaderProgram, const std::shared_ptr<const Material> material);
void RenderMeshDepth(const std::shared_ptr<const Mesh> mesh);
void RenderModel(const std::shared_ptr<const Model> model, ShaderProgram shaderProgram);
void RenderModelDepth(const std::shared_ptr<const Model> model, ShaderProgram shaderProgram);
void RenderBillboard(const std::shared_ptr<const Billboard> billboard, ShaderProgram shaderProgram, TextureId textureId);
