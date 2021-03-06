
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <ImGui/imgui.h>
// #include <filewatch/FileWatch.hpp>

#include "Aliases.hpp"
#include "Benchmark.hpp"
#include "Billboard.hpp"
#include "Camera.hpp"
#include "Debug.hpp"
#include "FileWatcher.hpp"
#include "Framebuffer.hpp"
#include "Light.hpp"
#include "Material.hpp"
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

bool minimized = false;

std::shared_ptr<Scene> scene = std::make_shared<Scene>();

std::vector<std::shared_ptr<Material>> materials;

int currentPipeline = 0;
int currentViewMode = 0;

void CheckMinimized(WindowDimension width, WindowDimension height) { minimized = (width == 0 || height == 0); }

void SetRenderPipeline(int index)
{
    switch (currentPipeline)
    {
    case 0:
        for (const auto& material : materials)
        {
            SetMaterialShader(material, ResourceManager::GetInstance().GetDSGeometryShader());
        }

        // DeleteDSRenderData(dsRenderData);
        break;
    case 1:
        for (const auto& material : materials)
        {
            SetMaterialShader(material, ResourceManager::GetInstance().GetForwardSunShader());
        }
        break;
    }
}

int main()
{

    // PRN_STRUCT_OFFSETS(SunLight, position, direction, color, power);
    // PRN_STRUCT_OFFSETS(Texture, id, type, path, width, height, componentCount, isLoaded);
    // PRN_STRUCT_OFFSETS(Billboard, transform, shader, vbo, vao, texture);

    Window window = Window("OpenGL", 1240, 720);

    scene->camera = CreateCamera();

    ResourceManager::GetInstance().Initialize();

    scene->skybox       = LoadSkybox("Assets/Skyboxes/skybox");
    scene->ambientLight = glm::vec3(0.4f);

    std::shared_ptr<Material> gunMaterialPBR = CreateMaterial(ResourceManager::GetInstance().GetDSGeometryShader());
    SetMaterialTexture(gunMaterialPBR, "albedoMap",
                       LoadTexture("Assets/Models/9mmfbx/source/GunGS_Albedo.png", TextureType::Color, true).id);
    SetMaterialTexture(gunMaterialPBR, "normalMap",
                       LoadTexture("Assets/Models/9mmfbx/source/GunGS_NormalGL.png", TextureType::NonColor, true).id);
    SetMaterialTexture(gunMaterialPBR, "metalnessMap",
                       LoadTexture("Assets/Models/9mmfbx/source/GunGS_Metallic.png", TextureType::NonColor, true).id);
    SetMaterialTexture(gunMaterialPBR, "roughnessMap",
                       LoadTexture("Assets/Models/9mmfbx/source/GunGS_Roughness.png", TextureType::NonColor, true).id);
    SetMaterialTexture(gunMaterialPBR, "aoMap", LoadTexture("Assets/Models/9mmfbx/source/GunGS_AO.png", TextureType::NonColor, true).id);

    materials.push_back(gunMaterialPBR);

    std::shared_ptr<Material> metalMaterial = CreateMaterial(ResourceManager::GetInstance().GetDSGeometryShader());
    SetMaterialTexture(metalMaterial, "albedoMap",
                       LoadTexture("Assets/Materials/metalLined/rusting-lined-metal_albedo.png", TextureType::Color, true).id);
    SetMaterialTexture(metalMaterial, "normalMap",
                       LoadTexture("Assets/Materials/metalLined/rusting-lined-metal_normal-ogl.png", TextureType::NonColor, true).id);
    SetMaterialTexture(metalMaterial, "metalnessMap",
                       LoadTexture("Assets/Materials/metalLined/rusting-lined-metal_metallic.png", TextureType::NonColor, true).id);
    SetMaterialTexture(metalMaterial, "roughnessMap",
                       LoadTexture("Assets/Materials/metalLined/rusting-lined-metal_roughness.png", TextureType::NonColor, true).id);
    SetMaterialTexture(metalMaterial, "aoMap",
                       LoadTexture("Assets/Materials/metalLined/rusting-lined-metal_ao.png", TextureType::NonColor, true).id);

    materials.push_back(metalMaterial);

    std::shared_ptr<Model> gun    = LoadModel("Assets/Models/9mmfbx/source/9mm.fbx", gunMaterialPBR, "Gun", true);
    std::shared_ptr<Model> sphere = LoadModel("Assets/Models/sphere/sphere.obj", metalMaterial, "Sphere", true);
    std::shared_ptr<Model> cube   = LoadModel("Assets/Models/WoodenBox/cube.obj", metalMaterial, "Cube", true);

    cube->transform.position = glm::vec3(0.0, -2.0, 0.0);
    cube->transform.scale    = glm::vec3(10.0, 1.0, 10.0);

    sphere->transform.position = glm::vec3(0.0, 0.0, 3.0);

    SetMaterial(gun, gunMaterialPBR);
    SetMaterial(sphere, metalMaterial);
    SetMaterial(cube, metalMaterial);

    scene->models.push_back(gun);
    scene->models.push_back(sphere);
    scene->models.push_back(cube);

    // scene->pointLights.push_back({
    //     .position = {1.0f, 1.0f, 1.0f},
    //     .color    = {1.0f, 1.0f, 1.0f},
    //     .power    = 20.0f,
    // });

    // scene->pointLights.push_back({
    //     .position = {-1.0f, 1.0f, 1.0f},
    //     .color    = {1.0f, 0.0f, 0.0f},
    //     .power    = 20.0f,
    // });

    // scene->pointLights.push_back({
    //     .position = {1.0f, 1.0f, -1.0f},
    //     .color    = {0.0f, 1.0f, 0.0f},
    //     .power    = 20.0f,
    // });

    scene->sunLights.push_back({.position  = {2.0f, 2.0f, 2.0f},
                                .direction = glm::normalize(glm::vec3(3.0f, 3.0f, 3.0f)),
                                .color     = {1.0f, 1.0f, 1.0f},
                                .power     = 20.0f});
    // scene->sunLights.push_back(
    //     {.position = {4.0f, 4.0f, 4.0f}, .direction = {-3.0f, 4.0f, 2.0f}, .color = {1.0f, 1.0f, 1.0f}, .power = 20.0f});

    // float xPos = 0.0f;
    // for (auto& model : scene->models)
    // {
    //     model->transform.position.x = xPos;
    //     xPos += 4.0f;
    // }

    efsw::FileWatcher* fileWatcher = new efsw::FileWatcher();

    ShaderUpdateListener* shaderListener = new ShaderUpdateListener();

    efsw::WatchID watchID = fileWatcher->addWatch(
        (ResourceManager::GetInstance().GetRootPath() / "Assets/Shaders").make_preferred().string().c_str(), shaderListener, true);

    fileWatcher->watch();

    RenderData        renderData        = CreateRenderData(window.GetProperties().Width, window.GetProperties().Height);
    DSRenderData      dsRenderData      = CreateDSRenderData(window.GetProperties().Width, window.GetProperties().Height);
    ForwardRenderData forwardRenderData = CreateForwardRenderData(window.GetProperties().Width, window.GetProperties().Height);
    LightRenderData   lightRenderData   = CreateLightRenderData(2, 3);
    ShadowRenderData  shadowRenderData  = CreateShadowRenderData(lightRenderData);
    DebugRenderData   debugRenderData   = CreateDebugRenderData(dsRenderData);

    ResourceManager::GetInstance().AddFramebufferToResize(&dsRenderData.gBuffer);
    ResourceManager::GetInstance().AddFramebufferToResize(&renderData.hdrFramebuffer);

    window.AddFramebufferResizeCallback(ResizeFramebuffers);
    window.AddFramebufferResizeCallback(CheckMinimized);

    std::vector<const char*> renderingPipelines = {"Deferred + Forward Shading", "Forward Shading"};
    std::vector<const char*> viewModes          = {"Lit", "CSM"};

    int numPipelines = renderingPipelines.size();
    int numViewModes = viewModes.size();

    BenchmarkData benchmark = {.duration = 5.0f, .renderingPipelines = renderingPipelines};

    while (window.IsRunning())
    {
        window.Update();

        if (!minimized)
        {
            UpdateCamera(scene->camera, window);

            ImGui::Begin("Controls");

            ImGui::Text("F Switch to view mode");
            ImGui::Text("G Switch to mouse mode");
            ImGui::Text("WASD Move around in view mode");
            ImGui::Text("SHIFT Move down in view mode");
            ImGui::Text("SPACE Move up in view mode");

            ImGui::End();

            ImGui::Begin("Stats");

            ImGui::LabelText("FPS", "%.1f", 1.0f / window.GetDeltaTime());

            static int numPipelinesBenchmark = 1;

            static glm::vec3 camPos   = {7.2, 4.2, -5.6};
            static float     camYaw   = 140;
            static float     camPitch = -25;

            ImGui::DragInt("Pipelines To Benchmark", &numPipelinesBenchmark);

            if (ImGui::TreeNode("Camera Settings"))
            {
                ImGui::DragFloat3("Position", (float*)(&camPos));
                ImGui::DragFloat("Yaw", &camYaw);
                ImGui::DragFloat("Pitch", &camPitch);

                if (ImGui::Button("Set Current Camera Settings"))
                {
                    camPos   = scene->camera.position;
                    camYaw   = scene->camera.yaw;
                    camPitch = scene->camera.pitch;
                }
                ImGui::TreePop();
            }

            if (ImGui::Button(benchmark.isRunning ? "Running Benchmark..." : "Run Benchmark"))
            {
                benchmark.isRunning    = true;
                benchmark.numPipelines = numPipelinesBenchmark;

                SetCameraVectors(scene->camera, camPos, camPitch, camYaw);
                window.SetVSync(false);

                benchmark.results.clear();
                benchmark.beginTime = std::chrono::steady_clock::now();
            }

            for (const auto& result : benchmark.results)
            {
                if (ImGui::TreeNode(result.name))
                {
                    ImGui::LabelText("Total Time (s)", "%.2f", result.totalTime);
                    ImGui::LabelText("Average Time (ms)", "%.2f", result.averageTime);
                    ImGui::LabelText("Frame Count", "%i", result.frameCount);
                    ImGui::LabelText("Average FPS", "%.1f", result.averageFPS);
                    // ImGui::LabelText("Highest FPS", "%.1f", result.highestFPS);
                    // ImGui::LabelText("Lowest FPS", "%.1f", result.lowestFPS);

                    ImGui::TreePop();
                }
            }

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
                if (ImGui::TreeNode("Point Lights"))
                {
                    int lightIndex = 0;
                    for (auto& light : scene->pointLights)
                    {
                        if (ImGui::TreeNode(std::to_string(lightIndex).c_str()))
                        {
                            ImGui::DragFloat3("Position", (float*)(&light.position), 0.03f);

                            ImGui::ColorEdit3("Color", (float*)(&light.color));

                            ImGui::DragFloat("Power", &light.power, 0.05f);
                            ImGui::TreePop();

                            if (ImGui::TreeNode("Shadows"))
                            {
                                ImGui::DragFloat("Bias", &light.shadowBias, 0.001f);
                                ImGui::DragFloat("Near Clip", &light.shadowNearClip, 0.01f);
                                ImGui::DragFloat("Far Clip", &light.shadowFarClip, 1.0f);
                                ImGui::TreePop();
                            }
                        }
                        lightIndex++;
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Sun Lights"))
                {
                    int lightIndex = 0;
                    for (auto& light : scene->sunLights)
                    {
                        if (ImGui::TreeNode(std::to_string(lightIndex).c_str()))
                        {
                            ImGui::DragFloat3("Position", (float*)(&light.position), 0.03f);

                            glm::vec3 rotation = glm::degrees(light.direction);
                            ImGui::DragFloat3("Direction", (float*)(&rotation), 0.3f);
                            light.direction = glm::normalize(glm::radians(rotation));

                            ImGui::ColorEdit3("Color", (float*)(&light.color));

                            ImGui::DragFloat("Power", &light.power, 0.05f);

                            if (ImGui::TreeNode("Shadows"))
                            {
                                ImGui::DragFloat("Bias", &light.shadowBias, 0.001f);
                                ImGui::DragFloat("Z Multiplier", &light.zMult, 0.001f);
                                // ImGui::DragFloat("Near Clip", &light.shadowNearClip, 0.01f);
                                // ImGui::DragFloat("Far Clip", &light.shadowFarClip, 1.0f);
                                // ImGui::DragFloat("Ortho Size", &light.shadowMapOrtho, 1.0f);
                                ImGui::TreePop();
                            }
                            ImGui::TreePop();
                        }
                        lightIndex++;
                    }
                    ImGui::TreePop();
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
                ImGui::DragFloat("Speed", &scene->camera.movementSettings.speed, 0.03f);
                ImGui::DragFloat("Near Clip", &scene->camera.nearClip, 0.01f);
                ImGui::DragFloat("Far Clip", &scene->camera.farClip, 1.0f);
                ImGui::DragFloat("Exposure", &scene->camera.exposure, 0.03f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Materials"))
            {
                int materialIndex = 0;
                for (auto& material : materials)
                {
                    if (ImGui::TreeNode(std::to_string(materialIndex).c_str()))
                    {
                        for (auto& textureUnifrom : material->textureUniforms)
                        {
                            ImGui::Text("%s", textureUnifrom.name.c_str());
                            ImGui::Image((void*)(intptr_t)textureUnifrom.textureId, ImVec2(52, 52), ImVec2(0, 1), ImVec2(1, 0));
                        }

                        ImGui::TreePop();
                    }
                    materialIndex++;
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Render Settings"))
            {
                ImGui::Combo("Render Pipeline", &currentPipeline, &renderingPipelines[0], numPipelines);
                if (ImGui::IsItemEdited())
                {
                    SetRenderPipeline(currentPipeline);
                }

                ImGui::Combo("View Mode", &currentViewMode, &viewModes[0], numViewModes);
                // if (ImGui::IsItemEdited())
                // {
                //     SetRenderPipeline(currentPipeline);
                // }

                static bool vSync = true;
                if (ImGui::Checkbox("VSync", &vSync))
                {
                    window.SetVSync(vSync);
                }

                ImGui::TreePop();
            }

            ImGui::End();

            ImGui::Begin("Debug");
            if (ImGui::TreeNode("G-Buffer"))
            {
                for (const auto& texture : dsRenderData.gBuffer.textures)
                {
                    if (ImGui::TreeNode(texture.debugName.c_str()))
                    {
                        ImGui::Image((void*)(intptr_t)texture.id, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }
            ImGui::End();

            // ImGui::ShowDemoWindow();

            ResourceManager::GetInstance().CheckDirtyShaders();

            if (benchmark.isRunning)
            {
                currentPipeline = RunBenchmark(benchmark, window, SetRenderPipeline);
            }

            SetModelMatrices(scene);

            switch (currentPipeline)
            {
            case 0:
                RenderShadowPass(scene, lightRenderData, shadowRenderData);
                RenderDSGeometryPass(scene, dsRenderData);

                switch (currentViewMode)
                {
                case 0:
                    RenderDSLightPass(scene, renderData, dsRenderData, shadowRenderData);
                    RenderDSForwardPass(scene, renderData, dsRenderData);
                    RenderTransparentPass(scene);
                    RenderPostProcessPass(scene, renderData);
                    break;
                case 1:
                    RenderShadowCascades(0, renderData, dsRenderData, debugRenderData, shadowRenderData);
                default:
                    break;
                }

                break;
            case 1:
                RenderShadowPass(scene, lightRenderData, shadowRenderData);
                RenderForward(scene, renderData, forwardRenderData, shadowRenderData);
                RenderTransparentPass(scene);
                RenderPostProcessPass(scene, renderData);
                break;
            }
        }

        window.Render();
    }

    return 0;
}
