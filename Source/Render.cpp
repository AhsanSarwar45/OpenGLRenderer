#include "Render.hpp"

#include <iostream>

#include <glad/glad.h>
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Material.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Scene.hpp"
#include "Texture.hpp"

ScreenQuad CreateScreenQuad()
{
    ScreenQuad screenQuad;

    float vertices[20] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &screenQuad.vao);
    glGenBuffers(1, &screenQuad.vbo);
    glBindVertexArray(screenQuad.vao);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuad.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    return screenQuad;
}

void RenderScreenQuad(const ScreenQuad& screenQuad)
{
    glBindVertexArray(screenQuad.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

DeferredRenderData CreateDeferredRenderData(const WindowDimension width, const WindowDimension height)
{
    DeferredRenderData data;
    data.geometryPassShader =
        LoadShaders({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/PBRDeferredGeometryPass.frag"},
                    "Geometry Pass");
    data.lightPassShader =
        LoadShaders({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/PBRDeferredLightPass.frag"},
                    "Light Pass", false);

    data.gBuffer    = CreatePBRGeometryBuffer(width, height);
    data.screenQuad = CreateScreenQuad();

    UseShaderProgram(data.lightPassShader);

    for (int i = 0; i < data.gBuffer.textures.size(); i++)
    {
        ShaderSetInt(data.lightPassShader, data.gBuffer.textures[i].name, i);
    }

    return data;
}

void RenderModel(const std::shared_ptr<const Model> model) { RenderModel(model, model->shaderProgram); }

void RenderModel(const std::shared_ptr<const Model> model, const ShaderProgram shaderProgram)
{
    Transform transform = model->transform;

    glm::mat4 rotation = glm::toMat4(glm::quat(transform.rotation));

    glm::mat4 modelMatrix =
        glm::translate(glm::mat4(1.0f), transform.position) * rotation * glm::scale(glm::mat4(1.0f), transform.scale);

    UseShaderProgram(shaderProgram);
    ShaderSetMat4(shaderProgram, "model", modelMatrix);

    // printf("%s\n", model->name);
    for (auto& mesh : model->meshes)
    {
        RenderMesh(mesh, shaderProgram, model->materials[mesh->materialId]);
    }
}

void RenderMesh(const std::shared_ptr<const Mesh> mesh, const ShaderProgram shaderProgram,
                const std::shared_ptr<const Material> material)
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

void RenderGeometryPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // todo remove
    glBindFramebuffer(GL_FRAMEBUFFER, data.gBuffer.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& model : scene->models)
    {
        RenderModel(model, data.geometryPassShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RenderLightPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data, glm::vec3 cameraPos)
{
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
    ShaderSetFloat3(data.lightPassShader, "viewPos", cameraPos);
    // finally render quad
    RenderScreenQuad(data.screenQuad);
}
void RenderForwardPass(const std::shared_ptr<const Scene> scene, const DeferredRenderData& data)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, data.gBuffer.id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

    glBlitFramebuffer(0, 0, data.gBuffer.frameBufferWidth, data.gBuffer.frameBufferHeight, 0, 0,
                      data.gBuffer.frameBufferWidth, data.gBuffer.frameBufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // for (const auto& pointLight : scene->pointLights)
    // {
    //     bulb->transform.position = pointLight.position;
    //     RenderModel(bulb);
    // }

    DrawSkybox(scene->skybox);
}
