#pragma once
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

struct WindowProperties
{
    std::string Name;
    uint32_t    Width;
    uint32_t    Height;

    double OffsetX;
    double OffsetY;
};

class Window
{

  public:
    Window(const std::string& name, uint32_t width, uint32_t height);
    ~Window();

    void Update();
    void Render();
    bool IsRunning() { return !glfwWindowShouldClose(m_Window); }

    WindowProperties GetProperties() const { return m_Properties; }
    float            GetDeltaTime() const { return m_DeltaTime; }
    bool             IsFocused() const { return m_Focused; }
    GLFWwindow*      GetWindowPtr() { return m_Window; }

  private:
    GLFWwindow*      m_Window;
    WindowProperties m_Properties;

    bool m_Focused;

    float m_DeltaTime = 0.0f; // time between current frame and last frame
    float m_LastFrame = 0.0f;
};