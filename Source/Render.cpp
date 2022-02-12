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
    DSRenderData dsRenderData = {
        .gBuffer            = CreateGeometryBuffer(width, height),
        .geometryPassShader = ResourceManager::GetInstance().GetDSGeometryShader(),
        .sunLightPassShader =
            LoadShader({"Assets/Shaders/Deferred/DeferredLightPass.vert", "Assets/Shaders/Deferred/DeferredSunLightPass.frag"},
                       "Sun Light Pass Shader", false),
        .pointLightPassShader =
            LoadShader({"Assets/Shaders/Deferred/DeferredLightPass.vert", "Assets/Shaders/Deferred/DeferredPointLightPass.frag"},
                       "Point Light Pass Shader", false),
        .ambientPassShader =
            LoadShader({"Assets/Shaders/Deferred/DeferredLightPass.vert", "Assets/Shaders/Deferred/DeferredAmbientPass.frag"},
                       "Ambient Pass Shader", false)};
    for (int i = 0; i < dsRenderData.gBuffer.textures.size(); i++)
    {
        BindTexture(dsRenderData.gBuffer.textures[i].id, 15 + i);
    }

    return dsRenderData;
}

ForwardRenderData CreateForwardRenderData(WindowDimension width, WindowDimension height)
{
    return {.forwardPassShader = ResourceManager::GetInstance().GetForwardSunShader()};
}

LightRenderData CreateLightRenderData(uint16_t maxSunLightCount, uint16_t maxPointLightCount)
{
    return {.sunLightData   = {.lightDataUB   = CreateUniformBufferVector<SunLightUniformData>(3, maxSunLightCount),
                             .maxLightCount = maxSunLightCount},
            .pointLightData = {

                .lightDataUB   = CreateUniformBufferVector<PointLightUniformData>(5, maxPointLightCount),
                .maxLightCount = maxPointLightCount,

            }};
}

ShadowRenderData CreateShadowRenderData(const LightRenderData& lightRenderData, TextureDimension sunShadowResolution,
                                        TextureDimension pointShadowResolution, uint8_t numShadowCascades)
{
    ShadowRenderData shadowRenderData = {
        .sunLightData =
            {
                .lightTransformsUB =
                    CreateUniformBufferVector<LightTransform>(2, (numShadowCascades + 1) * lightRenderData.sunLightData.maxLightCount),
                .shadowPassShader = ResourceManager::GetInstance().GetSunShadowShader(),
                .shadowFramebuffer =
                    CreateDepthArrayFramebuffer((numShadowCascades + 1) * lightRenderData.sunLightData.maxLightCount, sunShadowResolution),
            },
        .pointLightData         = {.lightTransformsUB =
                               CreateUniformBufferVector<LightTransform>(4, 6 * lightRenderData.pointLightData.maxLightCount),
                           .shadowPassShader = ResourceManager::GetInstance().GetPointShadowShader(),
                           .shadowFramebuffer =
                               CreateDepthCubemapArrayFramebuffer(lightRenderData.pointLightData.maxLightCount, pointShadowResolution)},
        .shadowCascadeDistances = std::vector<float>(numShadowCascades + 1),
        .numShadowCascades      = numShadowCascades};

    BindTextureArray(shadowRenderData.sunLightData.shadowFramebuffer.textures[0].id, 12);
    BindCubemapArray(shadowRenderData.pointLightData.shadowFramebuffer.textures[0].id, 13);

    return shadowRenderData;
}

DebugRenderData CreateDebugRenderData(const DSRenderData& dsRenderData)
{
    ShaderProgram viewShadowCascadesShader =
        LoadShader({"Assets/Shaders/Deferred/Debug/ShadowCascadesViz.vert", "Assets/Shaders/Deferred/Debug/ShadowCascadesViz.frag"},
                   "CSM Viz Shader", false);
    return {.viewShadowCascadesShader = viewShadowCascadesShader};
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
    ShaderSetMat4(shaderProgram, "model", model->transform.modelMatrix);

    // printf("%s\n", model->name);
    for (auto& mesh : model->meshes)
    {
        RenderMesh(mesh, shaderProgram, model->materials[mesh->materialId]);
    }
}

void RenderModelDepth(const std::shared_ptr<const Model> model, ShaderProgram shaderProgram)
{
    ShaderSetMat4(shaderProgram, "model", model->transform.modelMatrix);

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

    // for (int i = 0; i < material->textureUniforms.size(); i++)
    // {
    //     UnBindTexture(i);
    // }
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

    glDisable(GL_BLEND);
}

void RenderShadowCascades(uint8_t sunLightIndex, const RenderData& renderData, const DSRenderData& dsRenderData,
                          const DebugRenderData& debugRenderData, const ShadowRenderData& shadowRenderData)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    UseShaderProgram(debugRenderData.viewShadowCascadesShader);

    ShaderSetFloatArray(debugRenderData.viewShadowCascadesShader, "shadowCascadeDistances", shadowRenderData.numShadowCascades,
                        &shadowRenderData.shadowCascadeDistances[0]);
    ShaderSetInt(debugRenderData.viewShadowCascadesShader, "shadowCascadeCount", shadowRenderData.numShadowCascades);
    ShaderSetInt(debugRenderData.viewShadowCascadesShader, "lightIndex", sunLightIndex);

    RenderQuad(renderData.screenQuad);

    glEnable(GL_DEPTH_TEST);
}

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const DSRenderData& dsRenderData)
{
    glViewport(0, 0, dsRenderData.gBuffer.width, dsRenderData.gBuffer.height);

    glBindFramebuffer(GL_FRAMEBUFFER, dsRenderData.gBuffer.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UseShaderProgram(dsRenderData.geometryPassShader);

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

    // Ambient Pass
    UseShaderProgram(dsRenderData.ambientPassShader);
    ShaderSetFloat3(dsRenderData.ambientPassShader, "ambientLight", scene->ambientLight);

    RenderQuad(renderData.screenQuad);

    // Sun Light Pass

    UseShaderProgram(dsRenderData.sunLightPassShader);
    ShaderSetFloatArray(dsRenderData.sunLightPassShader, "shadowCascadeDistances", shadowRenderData.numShadowCascades,
                        &shadowRenderData.shadowCascadeDistances[0]);
    ShaderSetInt(dsRenderData.sunLightPassShader, "shadowCascadeCount", shadowRenderData.numShadowCascades);
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
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderData.hdrFramebuffer.fbo);

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

    for (const auto& model : scene->models)
    {
        RenderModel(model, shaderProgram);
    }

    DrawSkybox(scene->skybox);
    glDisable(GL_MULTISAMPLE);
}

void RenderShadowPass(const std::shared_ptr<const Scene> scene, LightRenderData& lightRenderData, ShadowRenderData& shadowRenderData)
{
    if (scene->sunLights.size() > 0)
    {
        shadowRenderData.sunLightData.lightTransformsUB.data.clear();

        for (int i = 0; i < scene->sunLights.size(); i++)
        {
            SunLight sunLight = scene->sunLights[i];

            std::vector<glm::mat4> sunLightMatrices = GetSunLightMatrices(sunLight, scene->camera, shadowRenderData);
            size_t                 numMatrices      = shadowRenderData.numShadowCascades + 1;

            for (size_t j = 0; j < numMatrices; j++)
            {
                shadowRenderData.sunLightData.lightTransformsUB.data[numMatrices * i + j].LightSpaceVPMatrix = sunLightMatrices[j];
            }

            lightRenderData.sunLightData.lightDataUB.data[i] = {.position   = {sunLight.position, 1.0f},
                                                                .direction  = {sunLight.direction, 1.0f},
                                                                .color      = {sunLight.color, 1.0f},
                                                                .power      = {sunLight.power},
                                                                .shadowBias = {sunLight.shadowBias}};
        }

        RenderShadows(scene, lightRenderData.sunLightData, shadowRenderData.sunLightData);
    }

    if (scene->pointLights.size() > 0)
    {
        shadowRenderData.pointLightData.lightTransformsUB.data.clear();

        for (int i = 0; i < scene->pointLights.size(); i++)
        {
            PointLight pointLight      = scene->pointLights[i];
            glm::mat4  lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, pointLight.shadowNearClip, pointLight.shadowFarClip);

            shadowRenderData.pointLightData.lightTransformsUB.data[6 * i + 0].LightSpaceVPMatrix =
                lightProjection *
                glm::lookAt(pointLight.position, pointLight.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0, 0.0));
            shadowRenderData.pointLightData.lightTransformsUB.data[6 * i + 1].LightSpaceVPMatrix =
                lightProjection *
                glm::lookAt(pointLight.position, pointLight.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0, 0.0));
            shadowRenderData.pointLightData.lightTransformsUB.data[6 * i + 2].LightSpaceVPMatrix =
                lightProjection *
                glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0, 0.0, 1.0));
            shadowRenderData.pointLightData.lightTransformsUB.data[6 * i + 3].LightSpaceVPMatrix =
                lightProjection *
                glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0, 0.0, -1.0));
            shadowRenderData.pointLightData.lightTransformsUB.data[6 * i + 4].LightSpaceVPMatrix =
                lightProjection *
                glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0, -1.0, 0.0));
            shadowRenderData.pointLightData.lightTransformsUB.data[6 * i + 5].LightSpaceVPMatrix =
                lightProjection *
                glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0, -1.0, 0.0));

            lightRenderData.pointLightData.lightDataUB.data[i] = {
                .position       = {pointLight.position, 1.0f},
                .color          = {pointLight.color, 1.0f},
                .power          = pointLight.power,
                .shadowBias     = pointLight.shadowBias,
                .shadowNearClip = pointLight.shadowNearClip,
                .shadowFarClip  = pointLight.shadowFarClip,
            };
        }

        RenderShadows(scene, lightRenderData.pointLightData, shadowRenderData.pointLightData);
    }
}

template <typename T>
void RenderShadows(const std::shared_ptr<const Scene> scene, LightData<T>& lightData, LightShadowData& shadowData)
{
    UploadUniformBufferVector(shadowData.lightTransformsUB);
    UploadUniformBufferVector(lightData.lightDataUB);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowData.shadowFramebuffer.fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowData.shadowFramebuffer.textures[0].id, 0);

    glViewport(0, 0, shadowData.shadowFramebuffer.width, shadowData.shadowFramebuffer.height);
    glClear(GL_DEPTH_BUFFER_BIT);

    UseShaderProgram(shadowData.shadowPassShader);

    for (const auto& model : scene->models)
    {
        RenderModelDepth(model, shadowData.shadowPassShader);
    }
}

template void RenderShadows<SunLightUniformData>(const std::shared_ptr<const Scene> scene, LightData<SunLightUniformData>& lightData,
                                                 LightShadowData& shadowData);
template void RenderShadows<PointLightUniformData>(const std::shared_ptr<const Scene> scene, LightData<PointLightUniformData>& lightData,
                                                   LightShadowData& shadowData);
