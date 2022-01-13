#pragma once
#include <functional>
#include <string>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Aliases.hpp"

struct WindowProperties
{
    std::string     Name;
    WindowDimension Width;
    WindowDimension Height;

    double OffsetX;
    double OffsetY;

    std::vector<std::function<void(TextureDimension width, TextureDimension height)>> m_FramebufferResizeCallbacks;
};

class Window
{
  public:
    Window(const std::string& name, WindowDimension width, WindowDimension height);
    ~Window();

    void Update();
    void Render();
    bool IsRunning() { return !glfwWindowShouldClose(m_Window); }

    inline WindowProperties GetProperties() const { return m_Properties; }
    inline float            GetDeltaTime() const { return m_DeltaTime; }
    inline bool             IsFocused() const { return m_Focused; }
    inline GLFWwindow*      GetWindowPtr() { return m_Window; }

    inline void SetVSync(bool value) { glfwSwapInterval(value); }

    inline void AddFramebufferResizeCallback(std::function<void(TextureDimension width, TextureDimension height)> function)
    {
        m_Properties.m_FramebufferResizeCallbacks.push_back(function);
    }

  private:
    GLFWwindow*      m_Window;
    WindowProperties m_Properties;

    bool m_Focused;

    float m_DeltaTime = 0.0f; // time between current frame and last frame
    float m_LastFrame = 0.0f;
};