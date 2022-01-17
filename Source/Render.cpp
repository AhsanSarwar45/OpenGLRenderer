#include "Render.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <memory>

#include <glad/glad.h>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Aliases.hpp"
#include "Billboard.hpp"
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

DSRenderData CreateDSRenderData(WindowDimension width, WindowDimension height)
{
    GeometryFramebuffer gBuffer      = CreateGeometryBuffer(width, height);
    const auto          initFunction = [gBuffer](ShaderProgram shaderProgram) { SetUpLightPassShader(shaderProgram, gBuffer.textures); };
    ShaderProgram       lightPassShader =
        LoadShader({"../Assets/Shaders/Deferred/DeferredLightPass.vert", "../Assets/Shaders/Deferred/DeferredLightPass.frag"}, "Light Pass",
                   false, initFunction);
    return {
        .gBuffer            = gBuffer,
        .screenQuad         = ResourceManager::GetInstance().GetScreenQuad(),
        .geometryPassShader = ResourceManager::GetInstance().GetDSGeometryShader(),
        .lightPassShader    = lightPassShader,
    };
}

ForwardRenderData CreateForwardRenderData(WindowDimension width, WindowDimension height, TextureDimension shadowResolution)
{
    return {.forwardPassShader = ResourceManager::GetInstance().GetForwardLitShader(),
            .depthPassShader   = ResourceManager::GetInstance().GetShadowShader(),
            .depthFramebuffer  = Create3DDepthFramebuffer(shadowResolution),
            .width             = width,
            .height            = height};
}

void ResizeForwardViewport(const std::shared_ptr<ForwardRenderData> renderData, TextureDimension width, TextureDimension height)
{
    renderData->width  = width;
    renderData->height = height;
}

void DeleteDSRenderData(const std::shared_ptr<const DSRenderData> renderData)
{
    const DSRenderData data = *renderData.get();
    DeleteGeometryFramebuffer(data.gBuffer);
    // DeleteQuad(data.screenQuad);
    // ShaderInternal::DeleteShaderProgram(data.geometryPassShader);
    ShaderInternal::DeleteShaderProgram(data.lightPassShader);
}

void SetUpLightPassShader(ShaderProgram lightPassShader, const std::vector<FramebufferTexture>& textures)
{
    UseShaderProgram(lightPassShader);

    for (int i = 0; i < textures.size(); i++)
    {
        ShaderSetInt(lightPassShader, textures[i].name, i);
    }
}

void RenderModel(const std::shared_ptr<const Model> model, ShaderProgram shaderProgram)
{
    Transform transform = model->transform;

    glm::mat4 rotation = glm::toMat4(glm::quat(transform.rotation));

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), transform.position) * rotation * glm::scale(glm::mat4(1.0f), transform.scale);

    // printf("%s\n", model->name);
    for (auto& mesh : model->meshes)
    {
        std::shared_ptr<Material> material = model->materials[mesh->materialId];
        UseShaderProgram(material->shaderProgram);
        ShaderSetMat4(material->shaderProgram, "model", modelMatrix);

        RenderMesh(mesh, material->shaderProgram, material);
    }
}

void RenderMesh(const std::shared_ptr<const Mesh> mesh, ShaderProgram shaderProgram, const std::shared_ptr<const Material> material)
{

    int index = 0;
    for (auto& textureUniform : material->textureUniforms)
    {
        // printf("%d: %s\n", mesh->materialId, texture.name);
        ShaderSetInt(shaderProgram, textureUniform.location, index);
        BindTexture(textureUniform.textureId, index);
        index++;
    }

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    for (int i = 0; i < material->textureUniforms.size(); i++)
    {
        UnBindTexture(i);
    }
}

void RenderBillboard(const std::shared_ptr<const Billboard> billboardPtr, ShaderProgram shaderProgram, TextureId textureId)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const Billboard billboard = *billboardPtr;

    ShaderSetFloat3(shaderProgram, "billboardPos", billboard.position);
    ShaderSetFloat2(shaderProgram, "billboardScale", billboard.scale);

    BindTexture(textureId, 0);
    ShaderSetInt(shaderProgram, "material.albedo", 0);

    glBindVertexArray(billboard.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
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

    SetSceneUniforms(scene, data.lightPassShader);

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

    DrawSkybox(scene->skybox);
}

void RenderTransparentPass(const std::shared_ptr<const Scene> scene)
{

    std::shared_ptr<Billboard> lightBillboard = ResourceManager::GetInstance().GetBillboard();

    ShaderProgram billboardShader         = ResourceManager::GetInstance().GetBillboardShader();
    TextureId     pointLightTexture       = ResourceManager::GetInstance().GetPointLightIcon();
    TextureId     directionalLightTexture = ResourceManager::GetInstance().GetSunLightIcon();

    UseShaderProgram(billboardShader);

    for (const auto& pointLight : scene->pointLights)
    {
        lightBillboard->position = pointLight.position;

        RenderBillboard(lightBillboard, billboardShader, pointLightTexture);
    }

    for (const auto& sunLight : scene->sunLights)
    {
        lightBillboard->position = sunLight.position;

        RenderBillboard(lightBillboard, billboardShader, directionalLightTexture);
    }
}

void RenderForward(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const ForwardRenderData> renderData)
{
    glViewport(0, 0, renderData->width, renderData->height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ShaderProgram shaderProgram = renderData->forwardPassShader;

    SetSceneUniforms(scene, shaderProgram);

    // glActiveTexture(GL_TEXTURE0 + 7);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->depthFramebuffer.depthTexture.id);

    for (const auto& model : scene->models)
    {
        RenderModel(model, shaderProgram);
    }

    DrawSkybox(scene->skybox);
}

void RenderForwardShadowPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const ForwardRenderData> renderData)
{

    float                  near_plane = 1.0f;
    float                  far_plane  = 25.0f;
    float                  width      = static_cast<float>(renderData->depthFramebuffer.depthTexture.width);
    float                  height     = static_cast<float>(renderData->depthFramebuffer.depthTexture.height);
    glm::mat4              shadowProj = glm::perspective(glm::radians(90.0f), width / height, near_plane, far_plane);
    std::vector<glm::mat4> shadowTransforms;
    glm::vec3              lightPos = scene->pointLights[0].position;
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    // 1. render scene to depth cubemap
    // --------------------------------
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, renderData->depthFramebuffer.framebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);

    ShaderProgram depthPassShader = renderData->depthPassShader;
    UseShaderProgram(depthPassShader);
    for (unsigned int i = 0; i < 6; ++i)
        ShaderSetMat4(depthPassShader, "shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    ShaderSetFloat(depthPassShader, "far_plane", far_plane);
    ShaderSetFloat3(depthPassShader, "lightPos", lightPos);

    for (const auto& model : scene->models)
    {
        RenderModel(model, depthPassShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
