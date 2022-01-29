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
    return {.gBuffer            = gBuffer,
            .screenQuad         = ResourceManager::GetInstance().GetScreenQuad(),
            .geometryPassShader = ResourceManager::GetInstance().GetDSGeometryShader(),
            .lightPassShader    = lightPassShader};
}

ForwardRenderData CreateForwardRenderData(WindowDimension width, WindowDimension height)
{
    return {.forwardPassShader = ResourceManager::GetInstance().GetForwardSunShader(), .width = width, .height = height};
}

LightRenderData CreateLightRenderData(uint16_t maxSunLightCount, uint16_t maxPointLightCount)
{
    return {.sunLightData   = {.lightTransformsUB = CreateUniformBufferVector<LightTransform>(2, maxSunLightCount),
                             .maxLightCount     = maxSunLightCount},
            .pointLightData = {
                .lightTransformsUB = CreateUniformBufferVector<LightTransform>(3, 6 * maxPointLightCount),
                .lightDataUB       = CreateUniformBufferVector<LightUniformBufferData>(4, maxPointLightCount),
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

void ResizeForwardViewport(ForwardRenderData* renderData, TextureDimension width, TextureDimension height)
{
    renderData->width  = width;
    renderData->height = height;
}

void DeleteDSRenderData(const DSRenderData& renderData)
{
    DeleteGeometryFramebuffer(renderData.gBuffer);
    // DeleteQuad(data.screenQuad);
    // ShaderInternal::DeleteShaderProgram(data.geometryPassShader);
    ShaderInternal::DeleteShaderProgram(renderData.lightPassShader);
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

void RenderDSGeometryPass(const std::shared_ptr<const Scene> scene, const DSRenderData& renderData)
{

    glViewport(0, 0, renderData.gBuffer.frameBufferWidth, renderData.gBuffer.frameBufferHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, renderData.gBuffer.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModel(model, renderData.geometryPassShader);
    }
}
void RenderDSLightPass(const std::shared_ptr<const Scene> scene, const DSRenderData& renderData, const ShadowRenderData& shadowRenderData)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < renderData.gBuffer.textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, renderData.gBuffer.textures[i].textureData.id);
    }

    SetSceneUniforms(scene, renderData.lightPassShader);

    BindTextureArray(shadowRenderData.sunLightData.shadowFramebuffer.depthTexture.id, 12);
    BindCubemapArray(shadowRenderData.pointLightData.shadowFramebuffer.depthTexture.id, 13);

    UseShaderProgram(renderData.lightPassShader);

    // finally render quad
    RenderQuad(renderData.screenQuad);
}
void RenderDSForwardPass(const std::shared_ptr<const Scene> scene, const DSRenderData& renderData)
{

    glBindFramebuffer(GL_READ_FRAMEBUFFER, renderData.gBuffer.id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

    glBlitFramebuffer(0, 0, renderData.gBuffer.frameBufferWidth, renderData.gBuffer.frameBufferHeight, 0, 0,
                      renderData.gBuffer.frameBufferWidth, renderData.gBuffer.frameBufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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

void RenderForward(const std::shared_ptr<const Scene> scene, const ForwardRenderData& renderData, const ShadowRenderData& shadowRenderData)
{
    glEnable(GL_MULTISAMPLE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, renderData.width, renderData.height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderProgram shaderProgram = renderData.forwardPassShader;

    SetSceneUniforms(scene, shaderProgram);

    BindTextureArray(shadowRenderData.sunLightData.shadowFramebuffer.depthTexture.id, 12);
    BindCubemapArray(shadowRenderData.pointLightData.shadowFramebuffer.depthTexture.id, 13);

    for (const auto& model : scene->models)
    {
        RenderModel(model, shaderProgram);
    }

    DrawSkybox(scene->skybox);
    glDisable(GL_MULTISAMPLE);
}

void RenderForwardShadowPass(const std::shared_ptr<const Scene> scene, const ForwardRenderData& renderData)
{
    // RenderShadowPass(scene, renderData->lightData, renderData->shadowData);
}

void RenderDeferredShadowPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<DSRenderData> renderData)
{
    // RenderShadowPass(scene, renderData->lightData, renderData->shadowData);
}

void RenderShadowPass(const std::shared_ptr<const Scene> scene, LightRenderData& lightRenderData, const ShadowRenderData& shadowRenderData)
{
    lightRenderData.sunLightData.lightTransformsUB.data.clear();
    lightRenderData.pointLightData.lightTransformsUB.data.clear();

    glm::mat4 lightProjection, lightView;

    for (int i = 0; i < scene->sunLights.size(); i++)
    {
        SunLight         sunLight    = scene->sunLights[i];
        ShadowProperties shadowProps = sunLight.shadowProps;
        lightProjection              = glm::ortho(-shadowProps.shadowMapOrtho, shadowProps.shadowMapOrtho, -shadowProps.shadowMapOrtho,
                                     shadowProps.shadowMapOrtho, shadowProps.shadowNearClip, shadowProps.shadowFarClip);
        lightView                    = glm::lookAt(sunLight.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

        lightRenderData.sunLightData.lightTransformsUB.data[i].LightSpaceVPMatrix = lightProjection * lightView;
    }

    UploadUniformBufferVector(lightRenderData.sunLightData.lightTransformsUB);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowRenderData.sunLightData.shadowFramebuffer.framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowRenderData.sunLightData.shadowFramebuffer.depthTexture.id, 0);

    UseShaderProgram(shadowRenderData.sunLightData.shadowPassShader);
    glViewport(0, 0, shadowRenderData.sunLightData.shadowFramebuffer.depthTexture.width,
               shadowRenderData.sunLightData.shadowFramebuffer.depthTexture.height);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModelDepth(model, shadowRenderData.sunLightData.shadowPassShader);
    }

    for (int i = 0; i < scene->pointLights.size(); i++)
    {
        PointLight       pointLight  = scene->pointLights[i];
        ShadowProperties shadowProps = pointLight.shadowProps;
        lightProjection              = glm::perspective(glm::radians(90.0f), 1.0f, shadowProps.shadowNearClip, shadowProps.shadowFarClip);

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

        lightRenderData.pointLightData.lightDataUB.data[i] = {.position      = pointLight.position,
                                                              .shadowFarClip = pointLight.shadowProps.shadowFarClip};
    }

    UploadUniformBufferVector(lightRenderData.pointLightData.lightTransformsUB);

    UploadUniformBufferVector(lightRenderData.pointLightData.lightDataUB);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowRenderData.pointLightData.shadowFramebuffer.framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowRenderData.pointLightData.shadowFramebuffer.depthTexture.id, 0);

    UseShaderProgram(shadowRenderData.pointLightData.shadowPassShader);
    glViewport(0, 0, shadowRenderData.pointLightData.shadowFramebuffer.depthTexture.width,
               shadowRenderData.pointLightData.shadowFramebuffer.depthTexture.height);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModelDepth(model, shadowRenderData.pointLightData.shadowPassShader);
    }
}
