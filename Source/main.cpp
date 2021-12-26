#include <iostream>

#include <ImGui/imgui.h>

#include "Texture.hpp"
#include "Window.hpp"

int main()
{
    Window window = Window("OpenGL", 1240, 720);

    Texture texture = Texture("../Assets/Images/image.jpg", TextureType::Color);

    while (window.IsRunning())
    {
        window.Update();

        ImGui::Begin("Parameters");

        ImGui::Image((void*)(intptr_t)texture.GetID(), ImVec2(512, 512));

        ImGui::End();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.Render();
    }

    return 0;
}
