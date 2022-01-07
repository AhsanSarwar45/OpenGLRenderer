#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <ImGui/imgui.h>

#include "Billboard.hpp"
#include "Camera.hpp"
#include "Debug.hpp"
#include "FileWatcher.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Render.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"
#include "Skybox.hpp"
#include "Texture.hpp"
#include "Window.hpp"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

std::vector<std::shared_ptr<Model>> models;
std::vector<PointLight>             pointLights;

glm::vec3 objPos(0.0f, 0.0f, 0.0f);
glm::vec3 lightPos(2.2f, 2.0f, 3.0f);
glm::vec3 lightDir(3.0f, 3.0f, 3.0f);

float objectShininess = 32.0f;

glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular(0.7f, 0.7f, 0.7f);
glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);

float lightLinear    = 0.01f;
float lightConstant  = 0.01f;
float lightQuadratic = 0.045f;

float shadowNearClip = 0.0f;
float shadowFarClip  = 10.0f;
float shadowMapOrtho = 5.0f;

int main()
{

    // PRN_STRUCT_OFFSETS(Model, name, transform, meshes, material, shader);
    // PRN_STRUCT_OFFSETS(Texture, id, type, path, width, height, componentCount, isLoaded);
    // PRN_STRUCT_OFFSETS(Billboard, transform, shader, vbo, vao, texture);

    Window window = Window("OpenGL", 1240, 720);
    Camera camera = Camera(&window);

    // std::unordered_map<std::string, Shader*> shaders;

    ResourceManager::GetInstance().Initialize();

    Billboard billboard = LoadBillboard("../Assets/Images/grass.png");

    ShaderProgram shaderProgram = LoadShaders({"../Assets/Shaders/Lit.vert", "../Assets/Shaders/Lit.frag"}, "Lit");
    ShaderProgram lightShaderProgram =
        LoadShaders({"../Assets/Shaders/Lit.vert", "../Assets/Shaders/Light.frag"}, "Light");
    // ShaderProgram depthShaderProgram = LoadShaders(
    //     {"../Assets/Shaders/SimpleDepthShader.vert", "../Assets/Shaders/SimpleDepthShader.frag"}, "Depth", false);
    // ShaderProgram shadowShaderProgram =
    //     LoadShaders({"../Assets/Shaders/ShadowMapLit.vert", "../Assets/Shaders/ShadowMapLit.frag"}, "LitShadow");
    ShaderProgram geometryPassShader =
        LoadShaders({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/DeferredGeometryPass.frag"},
                    "Geometry Pass");
    ShaderProgram lightPassShader = LoadShaders(
        {"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/DeferredLightPass.frag"}, "Light Pass", false);
    // Shader outlineShader = LoadShader("../Assets/Shaders/Lit.vert", "../Assets/Shaders/SingleColor.frag",
    // "Outline");

    Skybox skybox = LoadSkybox("../Assets/Skyboxes/skybox");

    // Model floor = LoadModelOBJ("../Assets/Models/WoodenBox/cube.obj", shadowShaderProgram, "Floor");
    std::shared_ptr<Model> bulb = LoadModel("../Assets/Models/WoodenBox/cube.obj", lightShaderProgram, "Bulb");

    bulb->transform.scale = glm::vec3(0.2f);

    // floor.transform.scale      = glm::vec3(20.0, 0.1, 20.0);
    // floor.transform.position.y = -0.5;

    // Model sponza   = LoadModelOBJ("../Assets/Models/sponza/sponza.obj", shadowShaderProgram, "Sponza");
    std::shared_ptr<Model> backpack =
        LoadModel("../Assets/Models/backpack/backpack.obj", geometryPassShader, "Backpack", false);

    // sponza.transform.scale = glm::vec3(0.05);

    // models.push_back(sponza);
    models.push_back(backpack);
    // models.push_back(backpack);
    // models.push_back(backpack);
    // models.push_back(backpack);
    // models.push_back(backpack);
    // models.push_back(LoadModelOBJ("../Assets/Models/african_head/african_head.obj", shadowShaderProgram, "Head"));
    // models.push_back(LoadModelOBJ("../Assets/Models/WoodenBox/cube.obj", shadowShaderProgram, "Box"));
    // models.push_back(LoadModelOBJ("../Assets/Models/backpack/backpack.obj", shadowShaderProgram, "Backpack", true));
    // models.push_back(floor);

    pointLights.push_back({.position  = glm::vec3(1.0f, 1.0f, 1.0f),
                           .color     = glm::vec3(1.0f, 1.0f, 1.0f),
                           .linear    = 0.01f,
                           .quadratic = 0.045f});

    pointLights.push_back({.position  = glm::vec3(-1.0f, 1.0f, 1.0f),
                           .color     = glm::vec3(1.0f, 0.0f, 1.0f),
                           .linear    = 0.01f,
                           .quadratic = 0.045f});

    pointLights.push_back({.position  = glm::vec3(2.0f, 1.0f, 1.0f),
                           .color     = glm::vec3(1.0f, 1.0f, 0.0f),
                           .linear    = 0.01f,
                           .quadratic = 0.045f});

    float xPos = 0.0f;
    for (auto& model : models)
    {
        model->transform.position.x = xPos;
        xPos += 4.0f;
    }

    efsw::FileWatcher* fileWatcher = new efsw::FileWatcher();

    ShaderUpdateListener* shaderListener = new ShaderUpdateListener();

    efsw::WatchID watchID = fileWatcher->addWatch("../Assets/Shaders", shaderListener, true);

    fileWatcher->watch();

    // DepthTexture depthMap = CreateDepthTexture(2048, 2048);

    // Framebuffer depthFramebuffer = CreateDepthFramebuffer(depthMap);

    BlinnPhongGeometryFramebuffer gBuffer =
        CreateBlinnPhongGeometryBuffer(window.GetProperties().Width, window.GetProperties().Height);

    ScreenQuad screenQuad = CreateScreenQuad();

    UseShaderProgram(lightPassShader);

    ShaderSetInt(lightPassShader, "gPosition", 0);
    ShaderSetInt(lightPassShader, "gNormal", 1);
    ShaderSetInt(lightPassShader, "gAlbedoSpec", 2);

    while (window.IsRunning())
    {
        window.Update();
        camera.Update(window.GetDeltaTime());

        ImGui::Begin("Stats");

        ImGui::LabelText("FPS", "%f", 1.0f / window.GetDeltaTime());

        ImGui::End();

        ImGui::Begin("Parameters");

        if (ImGui::TreeNode("Transforms"))
        {
            for (auto& model : models)
            {
                if (ImGui::TreeNode(model->name))
                {
                    ImGui::DragFloat3("Position", (float*)(&model->transform.position), 0.03f);
                    ImGui::DragFloat3("Scale", (float*)(&model->transform.scale), 0.03f);
                    glm::vec3 rotation = glm::degrees(model->transform.rotation);
                    ImGui::DragFloat3("Rotation", (float*)(&rotation), 0.3f);
                    model->transform.rotation = glm::radians(rotation);

                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Lights"))
        {
            int lightIndex = 0;
            for (auto& light : pointLights)
            {
                if (ImGui::TreeNode(std::to_string(lightIndex).c_str()))
                {
                    ImGui::DragFloat3("Position", (float*)(&light.position), 0.03f);
                    // glm::vec3 rotation = glm::degrees(lightDir);
                    // ImGui::DragFloat3("Direction", (float*)(&rotation), 0.3f);
                    // lightDir = glm::radians(rotation);

                    ImGui::ColorEdit3("Color", (float*)(&light.color));
                    // ImGui::ColorEdit3("Ambient", (float*)(&light.ambient));

                    // ImGui::DragFloat("Constant", &lightConstant, 0.003f, 0.01f);
                    ImGui::DragFloat("Linear", &light.linear, 0.003f, 0.01f);
                    ImGui::DragFloat("Quadratic", &light.quadratic, 0.003f, 0.01f);
                    ImGui::TreePop();
                }
                lightIndex++;
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Camera"))
        {
            ImGui::DragFloat("Speed", camera.GetSpeedPtr(), 0.03f);
            ImGui::DragFloat("Near Clip", camera.GetNearClipPtr(), 0.01f);
            ImGui::DragFloat("Far Clip", camera.GetFarClipPtr(), 1.0f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Shadows"))
        {
            ImGui::DragFloat("Near Clip", &shadowNearClip, 0.01f);
            ImGui::DragFloat("Far Clip", &shadowFarClip, 1.0f);
            ImGui::DragFloat("Ortho Size", &shadowMapOrtho, 1.0f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Material"))
        {
            // int total = -1;
            // glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &total);
            // for (int i = 0; i < total; ++i)
            // {
            //     int    name_len = -1, num = -1;
            //     GLenum type = GL_ZERO;
            //     char   name[100];
            //     glGetActiveUniform(shaderProgram, GLuint(i), sizeof(name) - 1, &name_len, &num, &type, name);
            //     name[name_len]  = 0;
            //     GLuint location = glGetUniformLocation(shaderProgram, name);

            //     std::string typeString;

            //     if (type == GL_FLOAT_MAT4)
            //         typeString = "mat4";
            //     else if (type == GL_FLOAT_VEC3)
            //         typeString = "vec3";
            //     else if (type == GL_FLOAT_VEC4)
            //         typeString = "vec4";
            //     else if (type == GL_FLOAT)
            //         typeString = "float";
            //     else if (type == GL_INT)
            //         typeString = "int";
            //     else if (type == GL_BOOL)
            //         typeString = "bool";
            //     else if (type == GL_SAMPLER_2D)
            //         typeString = "sampler2d";
            //     else
            //         typeString = type;

            //     ImGui::Text("Uniform Info Name: %s, Location: %d, Type:%s", name, location, typeString.c_str());
            // }
            ImGui::TreePop();
        }

        ImGui::End();

        ImGui::Begin("Debug");
        if (ImGui::TreeNode("G-Buffer"))
        {
            if (ImGui::TreeNode("Position"))
            {
                ImGui::Image((void*)(intptr_t)gBuffer.gPosition, ImVec2(256, 256), ImVec2(1, 1), ImVec2(0, 0));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Normal"))
            {
                ImGui::Image((void*)(intptr_t)gBuffer.gNormal, ImVec2(256, 256), ImVec2(1, 1), ImVec2(0, 0));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Albedo-Specular"))
            {
                ImGui::Image((void*)(intptr_t)gBuffer.gAlbedoSpec, ImVec2(256, 256), ImVec2(1, 1), ImVec2(0, 0));
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        ImGui::End();

        ResourceManager::GetInstance().CheckDirtyShaders();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.id);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto& model : models)
        {
            RenderModel(model, geometryPassShader);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        UseShaderProgram(lightPassShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpec);

        ShaderSetInt(lightPassShader, "numPointLights", pointLights.size());
        // send light relevant uniforms
        int index = 0;
        for (const auto& pointLight : pointLights)
        {
            std::string indexStr = std::to_string(index);
            ShaderSetFloat3(lightPassShader, "pointLights[" + indexStr + "].position", pointLight.position);
            ShaderSetFloat3(lightPassShader, "pointLights[" + indexStr + "].color", pointLight.color);
            ShaderSetFloat(lightPassShader, "pointLights[" + indexStr + "].linear", pointLight.linear);
            ShaderSetFloat(lightPassShader, "pointLights[" + indexStr + "].quadratic", pointLight.quadratic);

            index++;
        }
        ShaderSetFloat3(lightPassShader, "viewPos", camera.GetPosition());
        // finally render quad
        RenderScreenQuad(screenQuad);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

        WindowProperties props = window.GetProperties();
        glBlitFramebuffer(0, 0, props.Width, props.Height, 0, 0, props.Width, props.Height, GL_DEPTH_BUFFER_BIT,
                          GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        for (const auto& pointLight : pointLights)
        {
            bulb->transform.position = pointLight.position;
            RenderModel(bulb, lightShaderProgram);
        }

        DrawSkybox(skybox);

        // UseShaderProgram(shadowShaderProgram);

        // ShaderSetMat4(shadowShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);

        // ShaderSetFloat3(shadowShaderProgram, "lightPos", lightPos);
        // ShaderSetFloat3(shadowShaderProgram, "lightDirection", lightDir);

        // ShaderSetFloat3(shadowShaderProgram, "light.position", lightPos);
        // ShaderSetFloat3(shadowShaderProgram, "light.direction", lightDir);

        // ShaderSetFloat3(shadowShaderProgram, "light.diffuse", lightColor);
        // ShaderSetFloat3(shadowShaderProgram, "light.specular", lightSpecular);
        // ShaderSetFloat3(shadowShaderProgram, "light.ambient", lightAmbient);

        // ShaderSetFloat(shadowShaderProgram, "light.constant", lightConstant);
        // ShaderSetFloat(shadowShaderProgram, "light.linear", lightLinear);
        // ShaderSetFloat(shadowShaderProgram, "light.quadratic", lightQuadratic);

        // ShaderSetInt(shadowShaderProgram, "shadowMap", 7);
        // glActiveTexture(GL_TEXTURE7);
        // glBindTexture(GL_TEXTURE_2D, depthMap.id);

        // bulb.transform.position = lightPos;
        // DrawModel(bulb, lightShaderProgram);

        // for (auto& model : models)
        // {
        //     DrawModel(model);
        // }

        // glStencilMask(0x00);

        // glm::mat4 lightProjection, lightView;
        // glm::mat4 lightSpaceMatrix;
        // lightProjection =
        //     glm::ortho(-shadowMapOrtho, shadowMapOrtho, -shadowMapOrtho, shadowMapOrtho, shadowNearClip,
        //     shadowFarClip);
        // lightView        = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        // lightSpaceMatrix = lightProjection * lightView;
        // // render scene from light's point of view
        // UseShaderProgram(depthShaderProgram);
        // ShaderSetMat4(depthShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);

        // glViewport(0, 0, depthMap.width, depthMap.height);
        // glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);
        // glClear(GL_DEPTH_BUFFER_BIT);

        // for (auto& model : models)
        // {
        //     DrawModel(model, depthShaderProgram);
        // }
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // glViewport(0, 0, window.GetProperties().Width, window.GetProperties().Height);

        // glStencilFunc(GL_ALWAYS, 1, 0xFF);
        // glStencilMask(0xFF);

        // DrawBillboard(billboard);

        // Draw outline
        // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // glStencilMask(0x00);
        // glDisable(GL_DEPTH_TEST);

        // UseShaderProgram(outlineShader);
        // ShaderSetFloat3(outlineShader, "color", glm::vec3(1.0f, 1.0f, 0.0f));

        // for (auto model : models)
        // {
        //     model.transform.scale *= 1.1f;
        //     // std::cout << model.transform.scale.x << ", " << model.transform.scale.y << ", " <<
        //     // model.transform.scale.z
        //     //   << "\n";
        //     DrawModel(model, outlineShader);
        // }

        // glStencilMask(0xFF);
        // glStencilFunc(GL_ALWAYS, 1, 0xFF);
        // glEnable(GL_DEPTH_TEST);

        window.Render();
    }

    return 0;
}
