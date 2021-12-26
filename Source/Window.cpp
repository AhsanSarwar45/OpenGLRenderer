#define GLFW_INCLUDE_NONE
#include "Window.hpp"

#include <iostream>

#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/imgui.h>

void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

Window::Window(const std::string& name, uint32_t width, uint32_t height)
    : m_Properties({name, width, height, 0, 0})
{
    /* Initialize the library */
    glfwInit();

    glfwSetErrorCallback(ErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Create a windowed mode window and its OpenGL context */
    m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height,
                                m_Properties.Name.c_str(), NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_Window);

    glfwSetWindowUserPointer(m_Window, &m_Properties);

    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width,
                                                int height) {
        WindowProperties* data =
            static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));
        data->Width  = width;
        data->Height = height;

        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos,
                                          double yPos) {
        WindowProperties* data =
            static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));

        static double lastX = xPos;
        static double lastY = yPos;

        data->OffsetX = xPos - lastX;
        data->OffsetY = lastY - yPos; // reversed since y-coordinates go
                                      // from bottom to top

        lastX = xPos;
        lastY = yPos;
    });

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

Window::~Window()
{

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void Window::Update()
{
    float currentFrame = glfwGetTime();
    m_DeltaTime        = currentFrame - m_LastFrame;
    m_LastFrame        = currentFrame;

    if (glfwGetKey(m_Window, GLFW_KEY_F) == GLFW_PRESS)
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_Focused = true;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_G) == GLFW_PRESS)
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_Focused = false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    m_Properties.OffsetX = 0;
    m_Properties.OffsetY = 0;
    /* Swap front and back buffers */
    glfwSwapBuffers(m_Window);

    /* Poll for and process events */
    glfwPollEvents();
}
