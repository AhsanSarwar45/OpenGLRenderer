#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <ImGui/imgui.h>

#include "Billboard.hpp"
#include "Camera.hpp"
#include "Debug.hpp"
#include "FileWatcher.hpp"
#include "Model.hpp"
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

std::vector<Model> models;

glm::vec3 objPos(0.0f, 0.0f, 0.0f);
glm::vec3 lightPos(2.2f, 2.0f, 3.0f);
glm::vec3 lightDir(3.0f, 3.0f, 3.0f);

float objectShininess = 32.0f;

glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular(0.7f, 0.7f, 0.7f);
glm::vec3 lightAmbient(0.5f, 0.5f, 0.5f);

float lightLinear    = 0.01f;
float lightConstant  = 0.01f;
float lightQuadratic = 0.045f;

float shadowNearClip = 0.5f;
float shadowFarClip  = 10.0f;

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
    ShaderProgram depthShaderProgram = LoadShaders(
        {"../Assets/Shaders/SimpleDepthShader.vert", "../Assets/Shaders/SimpleDepthShader.frag"}, "Depth", false);
    ShaderProgram shadowShaderProgram =
        LoadShaders({"../Assets/Shaders/ShadowMapLit.vert", "../Assets/Shaders/ShadowMapLit.frag"}, "LitShadow");
    // Shader outlineShader = LoadShader("../Assets/Shaders/Lit.vert", "../Assets/Shaders/SingleColor.frag", "Outline");

    Skybox skybox = LoadSkybox("../Assets/Skyboxes/skybox");

    Model floor = LoadModelOBJ("../Assets/Models/WoodenBox/cube.obj", shadowShaderProgram, "Floor");
    Model bulb  = LoadModelOBJ("../Assets/Models/WoodenBox/cube.obj", shadowShaderProgram, "Bulb");

    bulb.transform.scale = glm::vec3(0.2f);

    floor.transform.scale      = glm::vec3(20.0, 0.1, 20.0);
    floor.transform.position.y = -0.5;

    models.push_back(LoadModelOBJ("../Assets/Models/african_head/african_head.obj", shadowShaderProgram, "Head"));
    models.push_back(LoadModelOBJ("../Assets/Models/WoodenBox/cube.obj", shadowShaderProgram, "Box"));
    models.push_back(LoadModelOBJ("../Assets/Models/backpack/backpack.obj", shadowShaderProgram, "Backpack", true));

    models.push_back(floor);

    float xPos = 0.0f;
    for (auto& model : models)
    {
        model.transform.position.x = xPos;
        xPos += 2.0f;
    }

    efsw::FileWatcher* fileWatcher = new efsw::FileWatcher();

    ShaderUpdateListener* shaderListener = new ShaderUpdateListener();

    efsw::WatchID watchID = fileWatcher->addWatch("../Assets/Shaders", shaderListener, true);

    fileWatcher->watch();

    bool showShadowMap = false;

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
                if (ImGui::TreeNode(model.name))
                {
                    ImGui::DragFloat3("Position", (float*)(&model.transform.position), 0.03f);
                    ImGui::DragFloat3("Scale", (float*)(&model.transform.scale), 0.03f);
                    ImGui::DragFloat3("Rotation", (float*)(&model.transform.rotation), 0.03f);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Light"))
        {
            ImGui::DragFloat3("Position", (float*)(&lightPos), 0.03f);
            ImGui::DragFloat3("Direction", (float*)(&lightDir), 0.03f);

            ImGui::ColorEdit3("Color", (float*)(&lightColor));
            ImGui::ColorEdit3("Specular", (float*)(&lightSpecular));
            ImGui::ColorEdit3("Ambient", (float*)(&lightAmbient));

            ImGui::DragFloat("Constant", &lightConstant, 0.003f, 0.01f);
            ImGui::DragFloat("Linear", &lightLinear, 0.003f, 0.01f);
            ImGui::DragFloat("Quadratic", &lightQuadratic, 0.003f, 0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Camera"))
        {
            ImGui::DragFloat("Near Clip", camera.GetNearClipPtr(), 0.01f);
            ImGui::DragFloat("Far Clip", camera.GetFarClipPtr(), 1.0f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Shadows"))
        {
            ImGui::DragFloat("Near Clip", &shadowNearClip, 0.01f);
            ImGui::DragFloat("Far Clip", &shadowFarClip, 1.0f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Material"))
        {
            int total = -1;
            glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &total);
            for (int i = 0; i < total; ++i)
            {
                int    name_len = -1, num = -1;
                GLenum type = GL_ZERO;
                char   name[100];
                glGetActiveUniform(shaderProgram, GLuint(i), sizeof(name) - 1, &name_len, &num, &type, name);
                name[name_len]  = 0;
                GLuint location = glGetUniformLocation(shaderProgram, name);

                std::string typeString;

                if (type == GL_FLOAT_MAT4)
                    typeString = "mat4";
                else if (type == GL_FLOAT_VEC3)
                    typeString = "vec3";
                else if (type == GL_FLOAT_VEC4)
                    typeString = "vec4";
                else if (type == GL_FLOAT)
                    typeString = "float";
                else if (type == GL_INT)
                    typeString = "int";
                else if (type == GL_BOOL)
                    typeString = "bool";
                else if (type == GL_SAMPLER_2D)
                    typeString = "sampler2d";
                else
                    typeString = type;

                ImGui::Text("Uniform Info Name: %s, Location: %d, Type:%s", name, location, typeString.c_str());
            }
            ImGui::TreePop();
        }

        ImGui::End();

        ImGui::Begin("Debug");
        if (ImGui::TreeNode("Shadow Map"))
        {
            ImGui::Image((void*)(intptr_t)depthMap, ImVec2(512, 512), ImVec2(1, 1), ImVec2(0, 0));
            ImGui::TreePop();
        }
        ImGui::End();

        ResourceManager::GetInstance().CheckDirtyShaders();

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        lightProjection  = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowNearClip, shadowFarClip);
        lightView        = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        UseShaderProgram(depthShaderProgram);
        ShaderSetMat4(depthShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (auto& model : models)
        {
            DrawModel(model, depthShaderProgram);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, window.GetProperties().Width, window.GetProperties().Height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        UseShaderProgram(shadowShaderProgram);

        ShaderSetMat4(shadowShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);

        ShaderSetFloat(shadowShaderProgram, "material.shininess", objectShininess);

        ShaderSetFloat3(shadowShaderProgram, "viewPos", camera.GetPosition());

        ShaderSetFloat3(shadowShaderProgram, "light.position", lightPos);
        ShaderSetFloat3(shadowShaderProgram, "light.direction", lightDir);

        ShaderSetFloat3(shadowShaderProgram, "light.diffuse", lightColor);
        ShaderSetFloat3(shadowShaderProgram, "light.specular", lightSpecular);
        ShaderSetFloat3(shadowShaderProgram, "light.ambient", lightAmbient);

        ShaderSetFloat(shadowShaderProgram, "light.constant", lightConstant);
        ShaderSetFloat(shadowShaderProgram, "light.linear", lightLinear);
        ShaderSetFloat(shadowShaderProgram, "light.quadratic", lightQuadratic);

        ShaderSetInt(shadowShaderProgram, "shadowMap", 7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        bulb.transform.position = lightPos;
        DrawModel(bulb, lightShaderProgram);

        for (auto& model : models)
        {
            DrawModel(model);
        }

        // glStencilMask(0x00);
        DrawSkybox(skybox);

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
