#include "Render.hpp"

#include <memory>

#include <glad/glad.h>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Aliases.hpp"
#include "Framebuffer.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

void RenderQuad(const Quad& quad)
{
    glBindVertexArray(quad.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

DSRenderData CreateDSRenderData(const GeometryFramebuffer& gBuffer, ShaderProgram geometryPassShader, ShaderProgram lightPassShader,
                                WindowDimension width, WindowDimension height)
{
    DSRenderData data = {
        .gBuffer            = gBuffer,
        .screenQuad         = CreateQuad(),
        .geometryPassShader = geometryPassShader,
        .lightPassShader    = lightPassShader,
    };

    UseShaderProgram(data.lightPassShader);

    for (int i = 0; i < data.gBuffer.textures.size(); i++)
    {
        ShaderSetInt(data.lightPassShader, data.gBuffer.textures[i].name, i);
    }

    return data;
}
DSRenderData CreatePBRDSRenderData(WindowDimension width, WindowDimension height)
{
    return CreateDSRenderData(
        CreatePBRGeometryBuffer(width, height),
        LoadShaders({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/PBRDeferredGeometryPass.frag"}, "Geometry Pass"),
        LoadShaders({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/PBRDeferredLightPass.frag"}, "Light Pass", false),
        width, height);
}
DSRenderData CreateBPDSRenderData(WindowDimension width, WindowDimension height)
{
    return CreateDSRenderData(
        CreateBPGeometryBuffer(width, height),
        LoadShaders({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/BPDeferredGeometryPass.frag"}, "Geometry Pass"),
        LoadShaders({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/BPDeferredLightPass.frag"}, "Light Pass", false), width,
        height);
}

void DeleteDSRenderData(const std::shared_ptr<const DSRenderData> renderData)
{
    const DSRenderData data = *renderData.get();
    DeleteGeometryFramebuffer(data.gBuffer);
    DeleteQuad(data.screenQuad);
    ShaderInternal::DeleteShaderProgram(data.geometryPassShader);
    ShaderInternal::DeleteShaderProgram(data.lightPassShader);
}

void RenderModel(const std::shared_ptr<const Model> model) { RenderModel(model, model->shaderProgram); }

void RenderModel(const std::shared_ptr<const Model> model, ShaderProgram shaderProgram)
{
    Transform transform = model->transform;

    glm::mat4 rotation = glm::toMat4(glm::quat(transform.rotation));

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), transform.position) * rotation * glm::scale(glm::mat4(1.0f), transform.scale);

    UseShaderProgram(shaderProgram);
    ShaderSetMat4(shaderProgram, "model", modelMatrix);

    // printf("%s\n", model->name);
    for (auto& mesh : model->meshes)
    {
        RenderMesh(mesh, shaderProgram, model->materials[mesh->materialId]);
    }
}

void RenderMesh(const std::shared_ptr<const Mesh> mesh, ShaderProgram shaderProgram, const std::shared_ptr<const Material> material)
{

    int index = 0;
    for (auto& texture : material->textures)
    {
        // printf("%d: %s\n", mesh->materialId, texture.name);
        ShaderSetInt(shaderProgram, ("material." + std::string(texture.name)).c_str(), index);
        BindTexture(texture.id, index);
        index++;
    }

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    for (int i = 0; i < material->textures.size(); i++)
    {
        UnBindTexture(i);
    }
}

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData)
{
    const DSRenderData data = *renderData.get();
    glBindFramebuffer(GL_FRAMEBUFFER, data.gBuffer.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModel(model, data.geometryPassShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData)
{
    const DSRenderData data = *renderData.get();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    UseShaderProgram(data.lightPassShader);

    for (int i = 0; i < data.gBuffer.textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, data.gBuffer.textures[i].textureData.id);
    }

    ShaderSetInt(data.lightPassShader, "numPointLights", scene->pointLights.size());
    ShaderSetFloat3(data.lightPassShader, "ambientLight", scene->ambientLight);
    // send light relevant uniforms
    int index = 0;
    for (const auto& pointLight : scene->pointLights)
    {
        std::string indexStr = std::to_string(index);
        ShaderSetFloat3(data.lightPassShader, "pointLights[" + indexStr + "].position", pointLight.position);
        ShaderSetFloat3(data.lightPassShader, "pointLights[" + indexStr + "].color", pointLight.color);
        ShaderSetFloat(data.lightPassShader, "pointLights[" + indexStr + "].power", pointLight.power);
        ShaderSetFloat(data.lightPassShader, "pointLights[" + indexStr + "].linear", pointLight.linear);
        ShaderSetFloat(data.lightPassShader, "pointLights[" + indexStr + "].quadratic", pointLight.quadratic);

        index++;
    }
    ShaderSetFloat3(data.lightPassShader, "viewPos", scene->camera->GetPosition());
    // finally render quad
    RenderQuad(data.screenQuad);
}
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const DSRenderData> renderData)
{
    const DSRenderData data = *renderData.get();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, data.gBuffer.id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

    glBlitFramebuffer(0, 0, data.gBuffer.frameBufferWidth, data.gBuffer.frameBufferHeight, 0, 0, data.gBuffer.frameBufferWidth,
                      data.gBuffer.frameBufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // for (const auto& pointLight : scene->pointLights)
    // {
    //     bulb->transform.position = pointLight.position;
    //     RenderModel(bulb);
    // }

    DrawSkybox(scene->skybox);
}

void RenderBPForward(const std::shared_ptr<const Scene> scene, const ForwardRenderData& data)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ShaderProgram shaderProgram = data.forwardPassShader;

    UseShaderProgram(shaderProgram);

    ShaderSetInt(shaderProgram, "numPointLights", scene->pointLights.size());
    ShaderSetFloat3(shaderProgram, "ambientLight", scene->ambientLight);
    // send light relevant uniforms
    int index = 0;
    for (const auto& pointLight : scene->pointLights)
    {
        std::string indexStr = std::to_string(index);
        ShaderSetFloat3(shaderProgram, "pointLights[" + indexStr + "].position", pointLight.position);
        ShaderSetFloat3(shaderProgram, "pointLights[" + indexStr + "].color", pointLight.color);
        ShaderSetFloat(shaderProgram, "pointLights[" + indexStr + "].power", pointLight.power);
        ShaderSetFloat(shaderProgram, "pointLights[" + indexStr + "].linear", pointLight.linear);
        ShaderSetFloat(shaderProgram, "pointLights[" + indexStr + "].quadratic", pointLight.quadratic);

        index++;
    }
    ShaderSetFloat3(shaderProgram, "viewPos", scene->camera->GetPosition());

    for (const auto& model : scene->models)
    {
        RenderModel(model, shaderProgram);
    }

    DrawSkybox(scene->skybox);
}
