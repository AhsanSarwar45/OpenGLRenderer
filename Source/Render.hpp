#pragma once

#include <memory>

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

DeferredRenderData CreateDeferredRenderData(const WindowDimension width, const WindowDimension height);

void RenderGeometryPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data);
void RenderLightPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data, glm::vec3 cameraPos);
void RenderForwardPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data);

void RenderQuad(const Quad& screenQuad);
void RenderMesh(const std::shared_ptr<const Mesh> mesh, const ShaderProgram shaderProgram,
                const std::shared_ptr<const Material> material);
void RenderModel(const std::shared_ptr<const Model> model, const ShaderProgram shader);
void RenderModel(const std::shared_ptr<const Model> model);
