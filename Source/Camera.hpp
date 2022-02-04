
#pragma once

#include "glm/fwd.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Uniform.hpp"
#include "Window.hpp"

struct CameraMovementSettings
{
    float speed       = 5.0f;
    float sensitivity = 0.15f;
};

struct ViewProjection
{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec4 position;
};

struct Camera
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 front    = {0.0f, 0.0f, -1.0f};
    glm::vec3 up       = {0.0f, 1.0f, 0.0f};
    glm::vec3 right;
    glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};

    float yaw;
    float pitch;

    float zoom = 45.0f;

    float nearClip = 0.1f;
    float farClip  = 1000.0f;

    float exposure = 1.0f;

    CameraMovementSettings movementSettings;

    UniformBuffer<ViewProjection> uniformBuffer;
};

Camera CreateCamera();

void UpdateCamera(Camera& camera, const Window& window);

void UpdateCameraVectors(Camera& camera);

void SetCameraVectors(Camera& camera, const glm::vec3& position, float pitch, float yaw);

void ProcessCameraMouse(Camera& camera, float xoffset, float yoffset, bool constrainPitch = true);
void ProcessCameraKeyboard(Camera& camera, const Window& window);

// class Camera
// {
//   public:

//     void Update(float deltaTime);

//     glm::mat4 GetViewMatrix() { return m_View; }
//     glm::mat4 GetProjectionMatrix() { return m_Projection; }

//     float* GetNearClipPtr() { return &m_NearClip; }
//     float* GetFarClipPtr() { return &m_FarClip; }
//     float* GetSpeedPtr() { return &m_MovementSpeed; }

//     float     GetZoom() const { return m_Zoom; }
//     glm::vec3 GetPosition() const { return m_Position; }
//     float     GetPitch() const { return m_Pitch; }
//     float     GetYaw() const { return m_Yaw; }

//     void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
//     void ProcessMouseScroll(float yoffset)
//     {
//         m_Zoom -= (float)yoffset;
//         if (m_Zoom < 1.0f)
//             m_Zoom = 1.0f;
//         if (m_Zoom > 45.0f)
//             m_Zoom = 45.0f;
//     }

//   private:
//     void UpdateCameraVectors();

//     unsigned int m_UniformBuffer;
// };