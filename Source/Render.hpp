#pragma once

#include <functional>
#include <memory>
#include <stdint.h>
#include <variant>

#include "Aliases.hpp"
#include "Framebuffer.hpp"
#include "Light.hpp"
#include "Quad.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Uniform.hpp"
#include "glm/fwd.hpp"

struct Scene;
struct Model;
struct Material;
struct Mesh;
struct Billboard;

using RenderPass = std::function<void(const std::shared_ptr<const Scene>)>;

template <typename T>
struct LightData
{
    UniformBufferVector<T> lightDataUB;

    uint16_t maxLightCount;
};

struct LightRenderData
{
    LightData<SunLightUniformData>   sunLightData;
    LightData<PointLightUniformData> pointLightData;
};

struct LightShadowData
{
    UniformBufferVector<LightTransform> lightTransformsUB;
    ShaderProgram                       shadowPassShader;
    Framebuffer                         shadowFramebuffer;
};

struct ShadowRenderData
{
    LightShadowData sunLightData;
    LightShadowData pointLightData;

    std::vector<float> shadowCascadeLevels = {0.02f, 0.04f, 0.1f, 0.5f};
    std::vector<float> shadowCascadeDistances;
    uint8_t            numShadowCascades;
};

struct DebugRenderData
{
    ShaderProgram viewShadowCascadesShader;
};

struct RenderData
{
    Framebuffer hdrFramebuffer;
    Quad        screenQuad;

    ShaderProgram postProcessShader;
};

// Deferred Shading Render Data
struct DSRenderData
{
    Framebuffer gBuffer;

    ShaderProgram geometryPassShader;
    ShaderProgram sunLightPassShader;
    ShaderProgram pointLightPassShader;
    ShaderProgram ambientPassShader;
};

// Forward Shading Render Data
struct ForwardRenderData
{
    ShaderProgram forwardPassShader;
};

struct RenderingPipeline
{
    const char* name;
};

RenderData        CreateRenderData(WindowDimension width, WindowDimension height);
DSRenderData      CreateDSRenderData(WindowDimension width, WindowDimension height);
ForwardRenderData CreateForwardRenderData(WindowDimension width, WindowDimension height);
LightRenderData   CreateLightRenderData(uint16_t maxSunLightCount = 4, uint16_t maxPointLightCount = 4);
ShadowRenderData  CreateShadowRenderData(const LightRenderData& lightRenderData, TextureDimension sunShadowResolution = 1024,
                                         TextureDimension pointShadowResolution = 512, uint8_t numShadowCascades = 4);
DebugRenderData   CreateDebugRenderData(const DSRenderData& dsRenderData);

void SetUpLightPassShader(ShaderProgram lightPassShader, const std::vector<Texture>& textures);
void ResizeForwardViewport(ForwardRenderData* renderData, TextureDimension width, TextureDimension height);

void DeleteDSRenderData(const DSRenderData& renderData);

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const DSRenderData& dsRenderData);
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const RenderData& renderData, const DSRenderData& dsRenderData,
                       const ShadowRenderData& shadowRenderData);
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const RenderData& renderData, const DSRenderData& dsRenderData);

void RenderForward(const std::shared_ptr<const Scene> scene, const RenderData& renderData, const ForwardRenderData& forwardRenderData,
                   const ShadowRenderData& shadowRenderData);

void RenderShadowPass(const std::shared_ptr<const Scene> scene, LightRenderData& lightRenderData, ShadowRenderData& shadowRenderData);

void RenderTransparentPass(const std::shared_ptr<const Scene> scene);
void RenderPostProcessPass(const std::shared_ptr<const Scene> scene, const RenderData& renderData);

template <typename T>
void RenderShadows(const std::shared_ptr<const Scene> scene, LightData<T>& lightData, LightShadowData& shadowData);

void RenderQuad(const Quad& screenQuad);
void RenderQuadInstanced(const Quad& screenQuad, size_t count);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, ShaderProgram shaderProgram, const std::shared_ptr<const Material> material);
void RenderMeshDepth(const std::shared_ptr<const Mesh> mesh);
void RenderModel(const std::shared_ptr<const Model> model, ShaderProgram shaderProgram);
void RenderModelDepth(const std::shared_ptr<const Model> model, ShaderProgram shaderProgram);
void RenderBillboard(const std::shared_ptr<const Billboard> billboard, ShaderProgram shaderProgram, TextureId textureId);

void RenderShadowCascades(uint8_t sunLightIndex, const RenderData& renderData, const DSRenderData& dsRenderData,
                          const DebugRenderData& debugRenderData, const ShadowRenderData& shadowRenderData);
