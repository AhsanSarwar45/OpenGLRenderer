#include "Render.hpp"
#include "Light.hpp"
#include "Uniform.hpp"

#include <memory>

#include <glad/glad.h>
#include <iostream>
#include <vector>

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

using namespace ModelInternal;

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

ForwardRenderData CreateForwardRenderData(WindowDimension width, WindowDimension height, TextureDimension shadowResolution,
                                          uint16_t maxLightCount)
{
    return {.forwardPassShader = ResourceManager::GetInstance().GetForwardSunShader(),
            .shadowPassShader  = ResourceManager::GetInstance().GetShadowShader(),
            .shadowFramebuffer = CreateDepthArrayFramebuffer(maxLightCount, shadowResolution, shadowResolution),
            .width             = width,
            .height            = height,
            .maxLightCount     = maxLightCount,
            .lightTransforms   = std::vector<LightTransform>(maxLightCount),
            .lightTransformUB  = CreateUniformBuffer(2, maxLightCount * sizeof(LightTransform))};
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
    SetModelUniforms(model, shaderProgram);

    // printf("%s\n", model->name);
    for (auto& mesh : model->meshes)
    {
        RenderMesh(mesh, shaderProgram, model->materials[mesh->materialId]);
    }
}

void RenderModelDepth(const std::shared_ptr<const Model> model, ShaderProgram shaderProgram)
{
    SetModelUniforms(model, shaderProgram);

    // printf("%s\n", model->name);
    for (auto& mesh : model->meshes)
    {
        RenderMeshDepth(mesh);
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
    glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    for (int i = 0; i < material->textureUniforms.size(); i++)
    {
        UnBindTexture(i);
    }
}

void RenderMeshDepth(const std::shared_ptr<const Mesh> mesh)
{
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, renderData->width, renderData->height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderProgram shaderProgram = renderData->forwardPassShader;

    SetSceneUniforms(scene, shaderProgram);

    // glActiveTexture(GL_TEXTURE0 + 7);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->depthFramebuffer.depthTexture.id);

    BindTextureArray(renderData->shadowFramebuffer.depthTexture.id, 12);

    for (const auto& model : scene->models)
    {
        RenderModel(model, shaderProgram);
    }

    UnBindTextureArray(12);

    DrawSkybox(scene->skybox);
}

void RenderForwardShadowPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<ForwardRenderData> renderData)
{

    // float                  near_plane = 1.0f;
    // float                  far_plane  = 25.0f;
    // float                  width      = static_cast<float>(renderData->depthFramebuffer.depthTexture.width);
    // float                  height     = static_cast<float>(renderData->depthFramebuffer.depthTexture.height);
    // glm::mat4              shadowProj = glm::perspective(glm::radians(90.0f), width / height, near_plane, far_plane);
    // std::vector<glm::mat4> shadowTransforms;
    // glm::vec3              lightPos = scene->pointLights[0].position;
    // shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    // shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,
    // 0.0f))); shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f,
    // 0.0f, 1.0f))); shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f,
    // 0.0f, -1.0f))); shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,
    // -1.0f, 0.0f))); shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
    // glm::vec3(0.0f, -1.0f, 0.0f)));

    // // 1. render scene to depth cubemap
    // // --------------------------------
    // glViewport(0, 0, width, height);
    // glBindFramebuffer(GL_FRAMEBUFFER, renderData->depthFramebuffer.framebuffer);
    // glClear(GL_DEPTH_BUFFER_BIT);

    // ShaderProgram depthPassShader = renderData->depthPassShader;
    // UseShaderProgram(depthPassShader);
    // for (unsigned int i = 0; i < 6; ++i)
    //     ShaderSetMat4(depthPassShader, "shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    // ShaderSetFloat(depthPassShader, "far_plane", far_plane);
    // ShaderSetFloat3(depthPassShader, "lightPos", lightPos);

    // for (const auto& model : scene->models)
    // {
    //     RenderModel(model, depthPassShader);
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderData->lightTransforms.clear();

    glm::mat4 lightProjection, lightView;

    for (int i = 0; i < scene->sunLights.size(); i++)
    {
        SunLight sunLight = scene->sunLights[i];
        lightProjection   = glm::ortho(-sunLight.shadowMapOrtho, sunLight.shadowMapOrtho, -sunLight.shadowMapOrtho, sunLight.shadowMapOrtho,
                                     sunLight.shadowNearClip, sunLight.shadowFarClip);
        lightView         = glm::lookAt(sunLight.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

        renderData->lightTransforms[i].LightSpaceVPMatrix = lightProjection * lightView;
    }

    SetUniformBufferSubData(renderData->lightTransformUB, &renderData->lightTransforms[0],
                            renderData->maxLightCount * sizeof(LightTransform));

    glBindFramebuffer(GL_FRAMEBUFFER, renderData->shadowFramebuffer.framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderData->shadowFramebuffer.depthTexture.id, 0);
    UseShaderProgram(renderData->shadowPassShader);
    glViewport(0, 0, renderData->shadowFramebuffer.depthTexture.width, renderData->shadowFramebuffer.depthTexture.height);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModelDepth(model, renderData->shadowPassShader);
    }

    // ShaderSetInt(renderData->shadowPassShader, "shadowMap", 7);
    // glActiveTexture(GL_TEXTURE7);
    // glBindTexture(GL_TEXTURE_2D, depthMap.id);
}
