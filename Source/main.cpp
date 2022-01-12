#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <ImGui/imgui.h>

#include "Aliases.hpp"
#include "Billboard.hpp"
#include "Camera.hpp"
#include "Debug.hpp"
#include "FileWatcher.hpp"
#include "Framebuffer.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Render.hpp"
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "Skybox.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Window.hpp"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

float shadowNearClip = 0.0f;
float shadowFarClip  = 10.0f;
float shadowMapOrtho = 5.0f;

bool minimized = false;

std::shared_ptr<Scene> scene = std::make_shared<Scene>();

const char* renderingPipelines[] = {"PBR Deferred + Forward Shading", "BP Deferred + Forward Shading", "BP Forward Shading"};

int currentPipeline = 0;

void CheckMinimized(WindowDimension width, WindowDimension height) { minimized = (width == 0 || height == 0); }

int main()
{

    // PRN_STRUCT_OFFSETS(Model, name, transform, meshes, materials, shaderProgram);
    // PRN_STRUCT_OFFSETS(Texture, id, type, path, width, height, componentCount, isLoaded);
    // PRN_STRUCT_OFFSETS(Billboard, transform, shader, vbo, vao, texture);

    Window window = Window("OpenGL", 1240, 720);
    scene->camera = std::make_shared<Camera>(&window);

    ResourceManager::GetInstance().Initialize();

    ShaderProgram shaderProgram = LoadShaders({"../Assets/Shaders/ForwardLit.vert", "../Assets/Shaders/BPForwardLit.frag"}, "Light");
    // ShaderProgram depthShaderProgram = LoadShaders(
    //     {"../Assets/Shaders/SimpleDepthShader.vert", "../Assets/Shaders/SimpleDepthShader.frag"}, "Depth", false);
    // ShaderProgram shadowShaderProgram =
    //     LoadShaders({"../Assets/Shaders/ShadowMapLit.vert", "../Assets/Shaders/ShadowMapLit.frag"}, "LitShadow");
    // ShaderProgram geometryPassShader =
    //     LoadShaders({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/DeferredGeometryPass.frag"},
    //                 "Geometry Pass");
    // ShaderProgram lightPassShader = LoadShaders(
    //     {"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/DeferredLightPass.frag"}, "Light Pass",
    //     false);

    // Shader outlineShader = LoadShader("../Assets/Shaders/Lit.vert", "../Assets/Shaders/SingleColor.frag",
    // "Outline");

    scene->skybox       = LoadSkybox("../Assets/Skyboxes/skybox");
    scene->ambientLight = glm::vec3(0.4f);

    // Model floor = LoadModelOBJ("../Assets/Models/WoodenBox/cube.obj", shadowShaderProgram, "Floor");
    // std::shared_ptr<Model> bulb = LoadModel("../Assets/Models/WoodenBox/cube.obj", lightShaderProgram, "Bulb");

    // bulb->transform.scale = glm::vec3(0.2f);

    // floor.transform.scale      = glm::vec3(20.0, 0.1, 20.0);
    // floor.transform.position.y = -0.5;

    std::shared_ptr<Material> gunMaterialPBR = std::make_shared<Material>();
    gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Albedo.png", "albedo", true));
    gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_NormalGL.png", "normal", true));
    gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Metallic.png", "metalness", true));
    gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Roughness.png", "roughness", true));
    gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_AO.png", "ao", true));

    std::shared_ptr<Material> metalLinedPBR = std::make_shared<Material>();
    metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_albedo.png", "albedo", true));
    metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_normal-ogl.png", "normal", true));
    metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_metallic.png", "metalness", true));
    metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_roughness.png", "roughness", true));
    metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_ao.png", "ao", true));

    // Model sponza   = LoadModelOBJ("../Assets/Models/sponza/sponza.obj", shadowShaderProgram, "Sponza");
    // std::shared_ptr<Model> backpack =
    //     LoadModel("../Assets/Models/backpack/backpack.obj", geometryPassShader, "Backpack", false);
    std::shared_ptr<Model> gun    = LoadModel("../Assets/Models/9mmfbx/source/9mm.fbx", shaderProgram, "Gun", true);
    std::shared_ptr<Model> sphere = LoadModel("../Assets/Models/sphere/sphere.obj", shaderProgram, "Sphere", true);
    std::shared_ptr<Model> cube   = LoadModel("../Assets/Models/WoodenBox/cube.obj", shaderProgram, "Cube", true);

    SetMaterial(gun, gunMaterialPBR);
    SetMaterial(sphere, metalLinedPBR);
    SetMaterial(cube, metalLinedPBR);

    // sponza.transform.scale = glm::vec3(0.05);

    // models.push_back(sponza);
    // models.push_back(backpack);
    scene->models.push_back(gun);
    scene->models.push_back(sphere);
    scene->models.push_back(cube);

    scene->pointLights.push_back(
        {.position = glm::vec3(1.0f, 1.0f, 1.0f), .color = glm::vec3(1.0f, 1.0f, 1.0f), .power = 20.0f, .linear = 0.0f, .quadratic = 0.0f});

    scene->pointLights.push_back({.position  = glm::vec3(-1.0f, -1.0f, 1.0f),
                                  .color     = glm::vec3(1.0f, 1.0f, 1.0f),
                                  .power     = 20.0f,
                                  .linear    = 0.0f,
                                  .quadratic = 0.0f});

    scene->pointLights.push_back(
        {.position = glm::vec3(2.0f, 1.0f, 1.0f), .color = glm::vec3(1.0f, 1.0f, 1.0f), .power = 20.0f, .linear = 0.0f, .quadratic = 0.0f});

    float xPos = 0.0f;
    for (auto& model : scene->models)
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

    std::shared_ptr<DSRenderData> dsRenderData = std::make_shared<DSRenderData>();
    *dsRenderData                              = CreatePBRDSRenderData(window.GetProperties().Width, window.GetProperties().Height);
    ForwardRenderData forwardRenderData        = {.forwardPassShader = ResourceManager::GetInstance().GetBPForwardLitShader()};

    // auto boundResizeFunction = [DSRenderData](TextureDimension width, TextureDimension height) {
    //     ResizeFramebufferTextures(&DSRenderData, width, height);
    // };
    auto boundResizeFunction = std::bind(&ResizeFramebufferTextures, dsRenderData, std::placeholders::_1, std::placeholders::_2);

    window.AddFramebufferResizeCallback(boundResizeFunction);
    window.AddFramebufferResizeCallback(CheckMinimized);

    std::vector<RenderPass> renderPasses;

    const std::vector<RenderPass> deferredRenderPipeline = {
        [dsRenderData](const std::shared_ptr<const Scene> scene) { RenderDSGeometryPass(scene, dsRenderData); },
        [dsRenderData](const std::shared_ptr<const Scene> scene) { RenderDSLightPass(scene, dsRenderData); },
        [dsRenderData](const std::shared_ptr<const Scene> scene) { RenderDSForwardPass(scene, dsRenderData); }};

    renderPasses = deferredRenderPipeline;

    while (window.IsRunning())
    {
        window.Update();
        scene->camera->Update(window.GetDeltaTime());

        ImGui::Begin("Stats");

        ImGui::LabelText("FPS", "%f", 1.0f / window.GetDeltaTime());

        ImGui::End();

        ImGui::Begin("Parameters");

        if (ImGui::TreeNode("Transforms"))
        {
            for (auto& model : scene->models)
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
            for (auto& light : scene->pointLights)
            {
                if (ImGui::TreeNode(std::to_string(lightIndex).c_str()))
                {
                    ImGui::DragFloat3("Position", (float*)(&light.position), 0.03f);

                    ImGui::ColorEdit3("Color", (float*)(&light.color));

                    ImGui::DragFloat("Power", &light.power, 0.05f);
                    ImGui::DragFloat("Linear", &light.linear, 0.003f);
                    ImGui::DragFloat("Quadratic", &light.quadratic, 0.003f);
                    ImGui::TreePop();
                }
                lightIndex++;
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Environment"))
        {
            ImGui::ColorEdit3("Ambient Light", (float*)(&scene->ambientLight));

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Camera"))
        {
            ImGui::DragFloat("Speed", scene->camera->GetSpeedPtr(), 0.03f);
            ImGui::DragFloat("Near Clip", scene->camera->GetNearClipPtr(), 0.01f);
            ImGui::DragFloat("Far Clip", scene->camera->GetFarClipPtr(), 1.0f);
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
        if (ImGui::TreeNode("Render Settings"))
        {
            int prevPipeline = currentPipeline;
            ImGui::Combo("Render Pipeline", &currentPipeline, renderingPipelines,
                         (int)(sizeof(renderingPipelines) / sizeof(*(renderingPipelines))));
            if (ImGui::IsItemEdited())
            {

                // printf("%d", currentPipeline);
                switch (currentPipeline)
                {
                case 0:
                    DeleteDSRenderData(dsRenderData);
                    *dsRenderData = CreatePBRDSRenderData(window.GetProperties().Width, window.GetProperties().Height);
                    renderPasses  = deferredRenderPipeline;
                    break;
                case 1:
                    DeleteDSRenderData(dsRenderData);
                    *dsRenderData = CreateBPDSRenderData(window.GetProperties().Width, window.GetProperties().Height);
                    renderPasses  = deferredRenderPipeline;
                    break;
                case 2:
                    renderPasses = {
                        [forwardRenderData](const std::shared_ptr<const Scene> scene) { RenderBPForward(scene, forwardRenderData); }};
                }
            }

            ImGui::TreePop();
        }

        ImGui::End();

        ImGui::Begin("Debug");
        if (ImGui::TreeNode("G-Buffer"))
        {
            for (const auto& texture : dsRenderData->gBuffer.textures)
            {
                if (ImGui::TreeNode(texture.name))
                {
                    ImGui::Image((void*)(intptr_t)texture.textureData.id, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
        ImGui::End();

        // ImGui::ShowDemoWindow();

        ResourceManager::GetInstance().CheckDirtyShaders();

        if (!minimized)
        {
            for (const auto& pass : renderPasses)
            {
                pass(scene);
            }
        }

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
