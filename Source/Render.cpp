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

// DSRenderData CreateDSRenderData(const GeometryFramebuffer& gBuffer, ShaderProgram geometryPassShader, ShaderProgram lightPassShader,
//                                 WindowDimension width, WindowDimension height)
// {
//     return data;
// }
DSRenderData CreatePBRDSRenderData(WindowDimension width, WindowDimension height)
{
    GeometryFramebuffer gBuffer      = CreatePBRGeometryBuffer(width, height);
    const auto          initFunction = [gBuffer](ShaderProgram shaderProgram) { SetUpLightPassShader(shaderProgram, gBuffer.textures); };
    ShaderProgram lightPassShader = LoadShader({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/PBRDeferredLightPass.frag"},
                                               "Light Pass", false, initFunction);
    return {
        .gBuffer            = gBuffer,
        .screenQuad         = ResourceManager::GetInstance().GetScreenQuad(),
        .geometryPassShader = ResourceManager::GetInstance().GetPBR_DS_GeometryShader(),
        .lightPassShader    = lightPassShader,
    };
}
DSRenderData CreateBPDSRenderData(WindowDimension width, WindowDimension height)
{
    GeometryFramebuffer gBuffer      = CreateBPGeometryBuffer(width, height);
    const auto          initFunction = [gBuffer](ShaderProgram shaderProgram) { SetUpLightPassShader(shaderProgram, gBuffer.textures); };
    ShaderProgram lightPassShader = LoadShader({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/BPDeferredLightPass.frag"},
                                               "Light Pass", false, initFunction);
    return {
        .gBuffer            = gBuffer,
        .screenQuad         = ResourceManager::GetInstance().GetScreenQuad(),
        .geometryPassShader = ResourceManager::GetInstance().GetBP_DS_GeometryShader(),
        .lightPassShader    = lightPassShader,
    };
}

ForwardRenderData CreatePBRForwardRenderData() { return {.forwardPassShader = ResourceManager::GetInstance().GetPBR_ForwardLitShader()}; }

ForwardRenderData CreateBPForwardRenderData() { return {.forwardPassShader = ResourceManager::GetInstance().GetBP_ForwardLitShader()}; }

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

    // for (const auto& pointLight : scene->pointLights)
    // {
    //     bulb->transform.position = pointLight.position;
    //     RenderModel(bulb);
    // }

    DrawSkybox(scene->skybox);
}

void RenderForward(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const ForwardRenderData> renderData)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ShaderProgram shaderProgram = renderData->forwardPassShader;

    SetSceneUniforms(scene, shaderProgram);

    for (const auto& model : scene->models)
    {
        RenderModel(model, shaderProgram);
    }

    DrawSkybox(scene->skybox);
}

// void RenderForwardShadowPass(const std::shared_ptr<const Scene> scene, const std::shared_ptr<const ForwardRenderData> renderData) {

//     float     near_plane = 1.0f;
//     float     far_plane  = 25.0f;
//     glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
//     std::vector<glm::mat4> shadowTransforms;
//     shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//     shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,
//     0.0f))); shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f,
//     0.0f, 1.0f))); shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f,
//     0.0f, -1.0f))); shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,
//     -1.0f, 0.0f))); shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
//     glm::vec3(0.0f, -1.0f, 0.0f)));

//     // 1. render scene to depth cubemap
//     // --------------------------------
//     glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//     glClear(GL_DEPTH_BUFFER_BIT);
//     simpleDepthShader.use();
//     for (unsigned int i = 0; i < 6; ++i)
//         simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
//     simpleDepthShader.setFloat("far_plane", far_plane);
//     simpleDepthShader.setVec3("lightPos", lightPos);
//     renderScene(simpleDepthShader);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
// }
