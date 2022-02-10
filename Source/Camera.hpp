
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
};

struct CameraUniformData
{
    ViewProjection viewProjection;
    glm::vec4      position;
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
    float farClip  = 500.0f;

    float exposure = 1.0f;

    WindowDimension viewportWidth;
    WindowDimension viewportHeight;

    ViewProjection viewProjection;

    CameraMovementSettings movementSettings;

    UniformBuffer<CameraUniformData> uniformBuffer;
};

Camera CreateCamera();

void UpdateCamera(Camera& camera, const Window& window);

void UpdateCameraVectors(Camera& camera);

void SetCameraVectors(Camera& camera, const glm::vec3& position, float pitch, float yaw);

void ProcessCameraMouse(Camera& camera, float xoffset, float yoffset, bool constrainPitch = true);
void ProcessCameraKeyboard(Camera& camera, const Window& window);