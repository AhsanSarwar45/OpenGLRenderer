
#pragma once

#include "Window.hpp"
#include "glm/fwd.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
#define YAW         -90.0f
#define PITCH       0.0f
#define SPEED       10.0f
#define SENSITIVITY 0.15f
#define ZOOM        45.0f
#define NEAR_CLIP   0.1f
#define FAR_CLIP    1000.0f

class Camera
{
  public:
    float exposure = 1.0f;

    Camera(Window* window, float nearClip = NEAR_CLIP, float farClip = FAR_CLIP, glm::vec3 position = glm::vec3(0.0f, 0.0f, -2.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(Window* window, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
    ~Camera();

    void Update(float deltaTime);

    glm::mat4 GetViewMatrix() { return m_View; }
    glm::mat4 GetProjectionMatrix() { return m_Projection; }

    float* GetNearClipPtr() { return &m_NearClip; }
    float* GetFarClipPtr() { return &m_FarClip; }
    float* GetSpeedPtr() { return &m_MovementSpeed; }

    float     GetZoom() const { return m_Zoom; }
    glm::vec3 GetPosition() const { return m_Position; }
    float     GetPitch() const { return m_Pitch; }
    float     GetYaw() const { return m_Yaw; }

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset)
    {
        m_Zoom -= (float)yoffset;
        if (m_Zoom < 1.0f)
            m_Zoom = 1.0f;
        if (m_Zoom > 45.0f)
            m_Zoom = 45.0f;
    }

    static Camera* GetActiveCamera() { return s_ActiveCamera; }

    void SetCamera(const glm::vec3& position, float pitch, float yaw)
    {
        m_Position = position;
        m_Yaw      = yaw;
        m_Pitch    = pitch;

        UpdateCameraVectors();
    }

  private:
    void UpdateCameraVectors();

    Window*     m_Window;
    GLFWwindow* m_NativeWindow;

    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;
    // euler Angles
    float m_Yaw;
    float m_Pitch;
    // camera options
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;

    float m_NearClip;
    float m_FarClip;

    glm::mat4 m_Projection;
    glm::mat4 m_View;

    static Camera* s_ActiveCamera;

    unsigned int m_UniformBuffer;
};