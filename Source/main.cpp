
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

float shadowNearClip = 0.0f;
float shadowFarClip  = 10.0f;
float shadowMapOrtho = 5.0f;

bool minimized = false;

std::shared_ptr<Scene> scene = std::make_shared<Scene>();

int currentPipeline = 0;

void CheckMinimized(WindowDimension width, WindowDimension height) { minimized = (width == 0 || height == 0); }

int main()
{
    std::vector<std::shared_ptr<Material>> materials;
    // PRN_STRUCT_OFFSETS(Model, name, transform, meshes, materials, shaderProgram);
    // PRN_STRUCT_OFFSETS(Texture, id, type, path, width, height, componentCount, isLoaded);
    // PRN_STRUCT_OFFSETS(Billboard, transform, shader, vbo, vao, texture);

    Window window = Window("OpenGL", 1240, 720);

    scene->camera = std::make_shared<Camera>(&window);

    ResourceManager::GetInstance().Initialize();

    scene->skybox       = LoadSkybox("../Assets/Skyboxes/skybox");
    scene->ambientLight = glm::vec3(0.4f);

    // std::shared_ptr<Material> gunMaterialPBR = std::make_shared<Material>();
    // gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Albedo.png", "albedo", true));
    // gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_NormalGL.png", "normal", true));
    // gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Metallic.png", "metalness", true));
    // gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Roughness.png", "roughness", true));
    // gunMaterialPBR->textures.push_back(LoadTexture("../Assets/Models/9mmfbx/source/GunGS_AO.png", "ao", true));

    // std::shared_ptr<Material> metalLinedPBR = std::make_shared<Material>();
    // metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_albedo.png", "albedo", true));
    // metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_normal-ogl.png", "normal", true));
    // metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_metallic.png", "metalness", true));
    // metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_roughness.png", "roughness",
    // true)); metalLinedPBR->textures.push_back(LoadTexture("../Assets/Materials/metalLined/rusting-lined-metal_ao.png", "ao", true));

    std::shared_ptr<Material> gunMaterialPBR = CreateMaterial(ResourceManager::GetInstance().GetDSGeometryShader());
    SetMaterialTexture(gunMaterialPBR, "albedoMap", LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Albedo.png", true).id);
    SetMaterialTexture(gunMaterialPBR, "normalMap", LoadTexture("../Assets/Models/9mmfbx/source/GunGS_NormalGL.png", true).id);
    SetMaterialTexture(gunMaterialPBR, "metalnessMap", LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Metallic.png", true).id);
    SetMaterialTexture(gunMaterialPBR, "roughnessMap", LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Roughness.png", true).id);
    SetMaterialTexture(gunMaterialPBR, "aoMap", LoadTexture("../Assets/Models/9mmfbx/source/GunGS_AO.png", true).id);

    materials.push_back(gunMaterialPBR);

    // gunMaterialPBR->albedoMap                   = LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Albedo.png", "albedo", true).id;
    // gunMaterialPBR->normalMap                   = LoadTexture("../Assets/Models/9mmfbx/source/unGS_NormalGL.png", "normal", true).id;
    // gunMaterialPBR->roughnessMap                = LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Metallic.png", "metalness",
    // true).id; gunMaterialPBR->metalnessMap                = LoadTexture("../Assets/Models/9mmfbx/source/GunGS_Roughness.png",
    // "roughness", true).id; gunMaterialPBR->aoMap                       = LoadTexture("../Assets/Models/9mmfbx/source/GunGS_AO.png",
    // "ao", true).id;

    std::shared_ptr<Model> gun = LoadModel("../Assets/Models/9mmfbx/source/9mm.fbx", gunMaterialPBR, "Gun", true);
    // std::shared_ptr<Model> sphere = LoadModel("../Assets/Models/sphere/sphere.obj", shaderProgram, "Sphere", true);
    // std::shared_ptr<Model> cube   = LoadModel("../Assets/Models/WoodenBox/cube.obj", shaderProgram, "Cube", true);

    // cube->transform.position = glm::vec3(0.0, -2.0, 0.0);
    // cube->transform.scale    = glm::vec3(10.0, 1.0, 10.0);

    // sphere->transform.position = glm::vec3(0.0, 0.0, 3.0);

    SetMaterial(gun, gunMaterialPBR);
    // SetMaterial(sphere, metalLinedPBR);
    // SetMaterial(cube, metalLinedPBR);

    scene->models.push_back(gun);
    // scene->models.push_back(sphere);
    // scene->models.push_back(cube);

    scene->pointLights.push_back({
        .position = {1.0f, 1.0f, 1.0f},
        .color    = {1.0f, 1.0f, 1.0f},
        .power    = 20.0f,
    });

    // scene->pointLights.push_back({.position  = glm::vec3(-1.0f, -1.0f, 1.0f),
    //                               .color     = glm::vec3(1.0f, 1.0f, 1.0f),
    //                               .power     = 20.0f,
    //                               .linear    = defaultLinearAttenuation,
    //                               .quadratic = defaultQuadraticAttenuation});

    // scene->pointLights.push_back({.position  = glm::vec3(2.0f, 1.0f, 1.0f),
    //                               .color     = glm::vec3(1.0f, 1.0f, 1.0f),
    //                               .power     = 20.0f,
    //                               .linear    = defaultLinearAttenuation,
    //                               .quadratic = defaultQuadraticAttenuation});

    scene->sunLights.push_back(
        {.position = {2.0f, 2.0f, 2.0f}, .direction = {3.0f, 3.0f, 3.0f}, .color = {1.0f, 1.0f, 1.0f}, .power = 20.0f});

    // float xPos = 0.0f;
    // for (auto& model : scene->models)
    // {
    //     model->transform.position.x = xPos;
    //     xPos += 4.0f;
    // }

    efsw::FileWatcher* fileWatcher = new efsw::FileWatcher();

    ShaderUpdateListener* shaderListener = new ShaderUpdateListener();

    efsw::WatchID watchID = fileWatcher->addWatch("../Assets/Shaders", shaderListener, true);

    fileWatcher->watch();

    // filewatch::FileWatch<std::filesystem::path> watch(L"../Assets/Shaders",
    //                                                   [](const std::filesystem::path& path, const filewatch::Event change_type) {
    //                                                       std::wcout << std::filesystem::absolute(path) << L"\n";
    //                                                   });

    // DepthTexture depthMap = CreateDepthTexture(2048, 2048);

    // Framebuffer depthFramebuffer = CreateDepthFramebuffer(depthMap);

    std::shared_ptr<DSRenderData> dsRenderData           = std::make_shared<DSRenderData>();
    *dsRenderData                                        = CreateDSRenderData(window.GetProperties().Width, window.GetProperties().Height);
    std::shared_ptr<ForwardRenderData> forwardRenderData = std::make_shared<ForwardRenderData>();
    *forwardRenderData = CreateForwardRenderData(window.GetProperties().Width, window.GetProperties().Height, 256);

    // auto boundResizeFunction = [DSRenderData](TextureDimension width, TextureDimension height) {
    //     ResizeFramebufferTextures(&DSRenderData, width, height);
    // };
    auto dsBoundResizeFunction      = std::bind(&ResizeFramebufferTextures, dsRenderData, std::placeholders::_1, std::placeholders::_2);
    auto forwardBoundResizeFunction = std::bind(&ResizeForwardViewport, forwardRenderData, std::placeholders::_1, std::placeholders::_2);

    window.AddFramebufferResizeCallback(dsBoundResizeFunction);
    window.AddFramebufferResizeCallback(forwardBoundResizeFunction);
    window.AddFramebufferResizeCallback(CheckMinimized);

    std::vector<RenderPass> renderPasses;

    const std::vector<RenderPass> deferredRenderPipeline = {
        [dsRenderData](const std::shared_ptr<const Scene> scene) { RenderDSGeometryPass(scene, dsRenderData); },
        [dsRenderData](const std::shared_ptr<const Scene> scene) { RenderDSLightPass(scene, dsRenderData); },
        [dsRenderData](const std::shared_ptr<const Scene> scene) { RenderDSForwardPass(scene, dsRenderData); }, RenderTransparentPass};

    const std::vector<RenderPass> forwardRenderPipeline = {
        [forwardRenderData](const std::shared_ptr<const Scene> scene) { RenderForward(scene, forwardRenderData); }, RenderTransparentPass};

    renderPasses = deferredRenderPipeline;

    std::vector<RenderingPipeline> renderingPipelines = {{"Deferred + Forward Shading", deferredRenderPipeline},
                                                         {"Forward Shading", forwardRenderPipeline}};

    std::vector<const char*> pipelineNames;

    for (const auto& pipeline : renderingPipelines)
    {
        pipelineNames.push_back(pipeline.name);
    }

    int numPipelines = pipelineNames.size();

    BenchmarkData benchmark = {
        .dsRenderData = dsRenderData, .forwardRenderData = forwardRenderData, .duration = 5.0f, .renderingPipelines = renderingPipelines};

    while (window.IsRunning())
    {
        window.Update();
        scene->camera->Update(window.GetDeltaTime());

        ImGui::Begin("Stats");

        ImGui::LabelText("FPS", "%.1f", 1.0f / window.GetDeltaTime());

        if (ImGui::Button(benchmark.isRunning ? "Running Benchmark..." : "Run Benchmark"))
        {
            benchmark.isRunning = true;
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
                        light.direction = glm::radians(rotation);

                        ImGui::ColorEdit3("Color", (float*)(&light.color));

                        ImGui::DragFloat("Power", &light.power, 0.05f);
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
            int prevPipeline = currentPipeline;
            ImGui::Combo("Render Pipeline", &currentPipeline, &pipelineNames[0], numPipelines);
            if (ImGui::IsItemEdited())
            {

                renderPasses = renderingPipelines[currentPipeline].renderPasses;
                switch (currentPipeline)
                {
                case 0:
                    SetMaterialShader(gunMaterialPBR, ResourceManager::GetInstance().GetDSGeometryShader());
                    // DeleteDSRenderData(dsRenderData);
                    // *dsRenderData = CreateDSRenderData(window.GetProperties().Width, window.GetProperties().Height);
                    break;
                case 1:
                    // *forwardRenderData = CreateForwardRenderData(window.GetProperties().Width, window.GetProperties().Height, 256);
                    SetMaterialShader(gunMaterialPBR, ResourceManager::GetInstance().GetForwardLitShader());
                    break;
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

        if (benchmark.isRunning)
        {
            currentPipeline = RunBenchmark(benchmark, window, renderPasses);
        }

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
