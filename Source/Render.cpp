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
void RenderQuadInstanced(const Quad& quad, size_t count)
{
    glBindVertexArray(quad.vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);
    glBindVertexArray(0);
}

RenderData CreateRenderData(WindowDimension width, WindowDimension height)
{
    ShaderProgram postProcessShader =
        LoadShader({"Assets/Shaders/Deferred/DeferredPostProcessPass.vert", "Assets/Shaders/Deferred/DeferredPostProcessPass.frag"},
                   "Post Processing Pass Shader", false);
    return {
        .hdrFramebuffer    = CreateHDRFramebuffer(width, height),
        .screenQuad        = ResourceManager::GetInstance().GetScreenQuad(),
        .postProcessShader = postProcessShader, // TODO move to resource manager
    };
}

DSRenderData CreateDSRenderData(WindowDimension width, WindowDimension height)
{
    Framebuffer   gBuffer      = CreateGeometryBuffer(width, height);
    const auto    initFunction = [gBuffer](ShaderProgram shaderProgram) { SetUpLightPassShader(shaderProgram, gBuffer.textures); };
    ShaderProgram pointLightShader =
        LoadShader({"Assets/Shaders/Deferred/DeferredLightPass.vert", "Assets/Shaders/Deferred/DeferredPointLightPass.frag"},
                   "Point Light Pass Shader", false, initFunction);
    ShaderProgram sunLightShader =
        LoadShader({"Assets/Shaders/Deferred/DeferredLightPass.vert", "Assets/Shaders/Deferred/DeferredSunLightPass.frag"},
                   "Sun Light Pass Shader", false, initFunction);
    ShaderProgram ambientShader =
        LoadShader({"Assets/Shaders/Deferred/DeferredLightPass.vert", "Assets/Shaders/Deferred/DeferredAmbientPass.frag"},
                   "Ambient Pass Shader", false, initFunction);

    return {.gBuffer = gBuffer,

            .geometryPassShader   = ResourceManager::GetInstance().GetDSGeometryShader(),
            .sunLightPassShader   = sunLightShader,
            .pointLightPassShader = pointLightShader,
            .ambientPassShader    = ambientShader};
}

ForwardRenderData CreateForwardRenderData(WindowDimension width, WindowDimension height)
{
    return {.forwardPassShader = ResourceManager::GetInstance().GetForwardSunShader()};
}

LightRenderData CreateLightRenderData(uint16_t maxSunLightCount, uint16_t maxPointLightCount)
{
    return {.sunLightData   = {.lightTransformsUB = CreateUniformBufferVector<LightTransform>(2, maxSunLightCount),
                             .lightDataUB       = CreateUniformBufferVector<SunLightUniformData>(3, maxPointLightCount),
                             .maxLightCount     = maxSunLightCount},
            .pointLightData = {
                .lightTransformsUB = CreateUniformBufferVector<LightTransform>(4, 6 * maxPointLightCount),
                .lightDataUB       = CreateUniformBufferVector<PointLightUniformData>(5, maxPointLightCount),
                .maxLightCount     = maxPointLightCount,
            }};
}

ShadowRenderData CreateShadowRenderData(const LightRenderData& lightRenderData, TextureDimension shadowResolution)
{
    return {.sunLightData   = {.shadowPassShader  = ResourceManager::GetInstance().GetSunShadowShader(),
                             .shadowFramebuffer = CreateDepthArrayFramebuffer(lightRenderData.sunLightData.maxLightCount, shadowResolution,
                                                                              shadowResolution)},
            .pointLightData = {.shadowPassShader = ResourceManager::GetInstance().GetPointShadowShader(),
                               .shadowFramebuffer =
                                   CreateDepthCubemapArrayFramebuffer(lightRenderData.pointLightData.maxLightCount, shadowResolution)}};
}

void DeleteDSRenderData(const DSRenderData& renderData)
{
    DeleteGeometryFramebuffer(renderData.gBuffer);
    // DeleteQuad(data.screenQuad);
    // ShaderInternal::DeleteShaderProgram(data.geometryPassShader);
    ShaderInternal::DeleteShaderProgram(renderData.sunLightPassShader);
    ShaderInternal::DeleteShaderProgram(renderData.pointLightPassShader);
    ShaderInternal::DeleteShaderProgram(renderData.ambientPassShader);
}

void SetUpLightPassShader(ShaderProgram lightPassShader, const std::vector<Texture>& textures)
{
    UseShaderProgram(lightPassShader);

    for (int i = 0; i < textures.size(); i++)
    {
        ShaderSetInt(lightPassShader, textures[i].debugName, i);
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

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const DSRenderData& dsRenderData)
{
    glViewport(0, 0, dsRenderData.gBuffer.width, dsRenderData.gBuffer.height);

    glBindFramebuffer(GL_FRAMEBUFFER, dsRenderData.gBuffer.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModel(model, dsRenderData.geometryPassShader);
    }
}
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const RenderData& renderData, const DSRenderData& dsRenderData,
                       const ShadowRenderData& shadowRenderData)
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderData.hdrFramebuffer.fbo);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < dsRenderData.gBuffer.textures.size(); i++)
    {
        BindTexture(dsRenderData.gBuffer.textures[i].id, i);
    }

    // Ambient Pass
    UseShaderProgram(dsRenderData.ambientPassShader);
    ShaderSetFloat3(dsRenderData.ambientPassShader, "ambientLight", scene->ambientLight);

    RenderQuad(renderData.screenQuad);

    BindTextureArray(shadowRenderData.sunLightData.shadowFramebuffer.textures[0].id, 12);
    BindCubemapArray(shadowRenderData.pointLightData.shadowFramebuffer.textures[0].id, 13);

    // Sun Light Pass
    UseShaderProgram(dsRenderData.sunLightPassShader);
    RenderQuadInstanced(renderData.screenQuad, scene->sunLights.size());

    // Point Light Pass
    UseShaderProgram(dsRenderData.pointLightPassShader);
    RenderQuadInstanced(renderData.screenQuad, scene->pointLights.size());

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const RenderData& renderData, const DSRenderData& dsRenderData)
{

    glBindFramebuffer(GL_READ_FRAMEBUFFER, dsRenderData.gBuffer.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderData.hdrFramebuffer.fbo); // write to default framebuffer

    glBlitFramebuffer(0, 0, dsRenderData.gBuffer.width, dsRenderData.gBuffer.height, 0, 0, dsRenderData.gBuffer.width,
                      dsRenderData.gBuffer.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, renderData.hdrFramebuffer.fbo);

    DrawSkybox(scene->skybox);
}

void RenderPostProcessPass(const std::shared_ptr<const Scene> scene, const RenderData& renderData)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    BindTexture(renderData.hdrFramebuffer.textures[0].id, 0);

    UseShaderProgram(renderData.postProcessShader);

    ShaderSetFloat(renderData.postProcessShader, "exposure", scene->camera.exposure);

    RenderQuad(renderData.screenQuad);

    glEnable(GL_DEPTH_TEST);
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

void RenderForward(const std::shared_ptr<const Scene> scene, const RenderData& renderData, const ForwardRenderData& forwardRenderData,
                   const ShadowRenderData& shadowRenderData)
{
    glEnable(GL_MULTISAMPLE);

    glBindFramebuffer(GL_FRAMEBUFFER, renderData.hdrFramebuffer.fbo);

    glViewport(0, 0, renderData.hdrFramebuffer.width, renderData.hdrFramebuffer.height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderProgram shaderProgram = forwardRenderData.forwardPassShader;

    SetSceneUniforms(scene, shaderProgram);

    BindTextureArray(shadowRenderData.sunLightData.shadowFramebuffer.textures[0].id, 12);
    BindCubemapArray(shadowRenderData.pointLightData.shadowFramebuffer.textures[0].id, 13);

    for (const auto& model : scene->models)
    {
        RenderModel(model, shaderProgram);
    }

    DrawSkybox(scene->skybox);
    glDisable(GL_MULTISAMPLE);
}

void RenderShadowPass(const std::shared_ptr<const Scene> scene, LightRenderData& lightRenderData, const ShadowRenderData& shadowRenderData)
{
    lightRenderData.sunLightData.lightTransformsUB.data.clear();
    lightRenderData.pointLightData.lightTransformsUB.data.clear();

    glm::mat4 lightProjection, lightView;

    for (int i = 0; i < scene->sunLights.size(); i++)
    {
        SunLight sunLight = scene->sunLights[i];
        lightProjection   = glm::ortho(-sunLight.shadowMapOrtho, sunLight.shadowMapOrtho, -sunLight.shadowMapOrtho, sunLight.shadowMapOrtho,
                                     sunLight.shadowNearClip, sunLight.shadowFarClip);
        lightView         = glm::lookAt(sunLight.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

        lightRenderData.sunLightData.lightTransformsUB.data[i].LightSpaceVPMatrix = lightProjection * lightView;

        lightRenderData.sunLightData.lightDataUB.data[i] = {.position   = {sunLight.position, 1.0f},
                                                            .direction  = {sunLight.direction, 1.0f},
                                                            .color      = {sunLight.color, 1.0f},
                                                            .power      = {sunLight.power},
                                                            .shadowBias = {sunLight.shadowBias}};
    }

    UploadUniformBufferVector(lightRenderData.sunLightData.lightTransformsUB);
    UploadUniformBufferVector(lightRenderData.sunLightData.lightDataUB);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowRenderData.sunLightData.shadowFramebuffer.fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowRenderData.sunLightData.shadowFramebuffer.textures[0].id, 0);

    UseShaderProgram(shadowRenderData.sunLightData.shadowPassShader);
    glViewport(0, 0, shadowRenderData.sunLightData.shadowFramebuffer.width, shadowRenderData.sunLightData.shadowFramebuffer.height);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModelDepth(model, shadowRenderData.sunLightData.shadowPassShader);
    }

    for (int i = 0; i < scene->pointLights.size(); i++)
    {
        PointLight pointLight = scene->pointLights[i];
        lightProjection       = glm::perspective(glm::radians(90.0f), 1.0f, pointLight.shadowNearClip, pointLight.shadowFarClip);

        lightRenderData.pointLightData.lightTransformsUB.data[6 * i + 0].LightSpaceVPMatrix =
            lightProjection *
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0, 0.0));
        lightRenderData.pointLightData.lightTransformsUB.data[6 * i + 1].LightSpaceVPMatrix =
            lightProjection *
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0, 0.0));
        lightRenderData.pointLightData.lightTransformsUB.data[6 * i + 2].LightSpaceVPMatrix =
            lightProjection * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0, 0.0, 1.0));
        lightRenderData.pointLightData.lightTransformsUB.data[6 * i + 3].LightSpaceVPMatrix =
            lightProjection *
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0, 0.0, -1.0));
        lightRenderData.pointLightData.lightTransformsUB.data[6 * i + 4].LightSpaceVPMatrix =
            lightProjection *
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0, -1.0, 0.0));
        lightRenderData.pointLightData.lightTransformsUB.data[6 * i + 5].LightSpaceVPMatrix =
            lightProjection *
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0, -1.0, 0.0));

        lightRenderData.pointLightData.lightDataUB.data[i] = {.position       = {pointLight.position, 1.0f},
                                                              .color          = {pointLight.color, 1.0f},
                                                              .power          = pointLight.power,
                                                              .shadowBias     = pointLight.shadowBias,
                                                              .shadowNearClip = pointLight.shadowNearClip,
                                                              .shadowFarClip  = pointLight.shadowFarClip};
    }

    UploadUniformBufferVector(lightRenderData.pointLightData.lightTransformsUB);
    UploadUniformBufferVector(lightRenderData.pointLightData.lightDataUB);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowRenderData.pointLightData.shadowFramebuffer.fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowRenderData.pointLightData.shadowFramebuffer.textures[0].id, 0);

    UseShaderProgram(shadowRenderData.pointLightData.shadowPassShader);
    glViewport(0, 0, shadowRenderData.pointLightData.shadowFramebuffer.width, shadowRenderData.pointLightData.shadowFramebuffer.height);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModelDepth(model, shadowRenderData.pointLightData.shadowPassShader);
    }
}
