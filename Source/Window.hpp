#pragma once
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

using WindowDimension = uint16_t;

struct WindowProperties
{
    std::string     Name;
    WindowDimension Width;
    WindowDimension Height;

    double OffsetX;
    double OffsetY;
};

class Window
{
  public:
    Window(const std::string& name, WindowDimension width, WindowDimension height);
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