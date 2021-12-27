#include <iostream>

#include <ImGui/imgui.h>
#include <vector>

#include "Camera.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Skybox.hpp"
#include "Texture.hpp"
#include "Window.hpp"

int main()
{
    Window             window = Window("OpenGL", 1240, 720);
    Camera             camera = Camera(&window);
    std::vector<Model> models;

    glm::vec3 objPos(0.0f, 0.0f, 0.0f);
    glm::vec3 lightPos(2.2f, 2.0f, 3.0f);

    float objectShininess = 32.0f;

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 lightSpecular(0.7f, 0.7f, 0.7f);
    glm::vec3 lightAmbient(0.5f, 0.5f, 0.5f);

    float lightLinear    = 0.01f;
    float lightConstant  = 0.01f;
    float lightQuadratic = 0.045f;

    Texture texture = LoadTexture("../Assets/Images/image.jpg", TextureType::Color);

    Shader shader = LoadShader("../Assets/Shaders/Lit.vert", "../Assets/Shaders/Lit.frag", "Head");

    Skybox skybox = LoadSkybox("../Assets/Skyboxes/skybox");

    models.push_back(LoadModelOBJ("../Assets/Models/african_head/african_head.obj", "Head"));
    models.push_back(LoadModelOBJ("../Assets/Models/Gun/Gun.obj", "Gun"));

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
                if (ImGui::TreeNode(model.name.c_str()))
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

            ImGui::ColorEdit3("Color", (float*)(&lightColor));
            ImGui::ColorEdit3("Specular", (float*)(&lightSpecular));
            ImGui::ColorEdit3("Ambient", (float*)(&lightAmbient));

            ImGui::DragFloat("Constant", &lightConstant, 0.003f, 0.01f);
            ImGui::DragFloat("Linear", &lightLinear, 0.003f, 0.01f);
            ImGui::DragFloat("Quadratic", &lightQuadratic, 0.003f, 0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Material"))
        {
            ImGui::DragFloat("Object Shininess", &objectShininess, 0.03f);
            ImGui::TreePop();
        }
        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        UseShader(shader);

        ShaderSetFloat(shader, "material.shininess", objectShininess);

        ShaderSetFloat3(shader, "viewPos", camera.GetPosition());

        ShaderSetFloat3(shader, "light.position", lightPos);

        ShaderSetFloat3(shader, "light.diffuse", lightColor);
        ShaderSetFloat3(shader, "light.specular", lightSpecular);
        ShaderSetFloat3(shader, "light.ambient", lightAmbient);

        ShaderSetFloat(shader, "light.constant", lightConstant);
        ShaderSetFloat(shader, "light.linear", lightLinear);
        ShaderSetFloat(shader, "light.quadratic", lightQuadratic);

        ShaderSetMat4(shader, "model", glm::translate(glm::mat4(1.0f), objPos));

        for (auto& model : models)
        {
            DrawModel(model, shader);
        }

        DrawSkybox(skybox);

        window.Render();
    }

    return 0;
}
