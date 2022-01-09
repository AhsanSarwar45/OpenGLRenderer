#pragma once

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

struct DeferredRenderData
{

    GeometryFramebuffer gBuffer;
    Quad                screenQuad;

    ShaderProgram geometryPassShader;
    ShaderProgram lightPassShader;
};

DeferredRenderData CreateDeferredRenderData(const GeometryFramebuffer gBuffer, const ShaderProgram vert,
                                            const ShaderProgram frag, const WindowDimension width,
                                            const WindowDimension height);
DeferredRenderData CreatePBRDeferredRenderData(const WindowDimension width, const WindowDimension height);
DeferredRenderData CreateBPDeferredRenderData(const WindowDimension width, const WindowDimension height);

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data);
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data, glm::vec3 cameraPos);
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data);

void RenderQuad(const Quad& screenQuad);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, const ShaderProgram shaderProgram,
                const std::shared_ptr<const Material> material);
void RenderModel(const std::shared_ptr<const Model> model, const ShaderProgram shader);
void RenderModel(const std::shared_ptr<const Model> model);
