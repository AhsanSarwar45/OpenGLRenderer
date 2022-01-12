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
};

DSRenderData CreateDSRenderData(const GeometryFramebuffer& gBuffer, ShaderProgram vert, ShaderProgram frag, WindowDimension width,
                                WindowDimension height);
DSRenderData CreatePBRDSRenderData(WindowDimension width, WindowDimension height);
DSRenderData CreateBPDSRenderData(WindowDimension width, WindowDimension height);

void DeleteDSRenderData(const std::shared_ptr<const DSRenderData> renderData);

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData);

void RenderBPForward(const std::shared_ptr<const Scene> scene, const ForwardRenderData& data);

void RenderQuad(const Quad& screenQuad);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, ShaderProgram shaderProgram, const std::shared_ptr<const Material> material);
void RenderModel(const std::shared_ptr<const Model> model, ShaderProgram shader);
void RenderModel(const std::shared_ptr<const Model> model);
