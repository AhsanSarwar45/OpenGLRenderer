#include "Camera.hpp"
#include "Window.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Camera CreateCamera()
{
    Camera camera;

    camera.uniformBuffer = CreateUniformBuffer<ViewProjection>(0);

    UpdateCameraVectors(camera);

    return camera;
}

void UpdateCamera(Camera& camera, const Window& window)
{
    // todo abstractify
    GLFWwindow* nativeWindow = window.GetWindowPtr();
    if (glfwGetKey(nativeWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(nativeWindow, true);

    if (window.IsFocused())
    {
        ProcessCameraKeyboard(camera, window);
        ProcessCameraMouse(camera, window.GetProperties().OffsetX, window.GetProperties().OffsetY);
    }

    camera.uniformBuffer.data.viewMatrix = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
    WindowProperties props               = window.GetProperties();
    if (props.Height > 0) // TODO get rid
    {
        camera.uniformBuffer.data.projectionMatrix =
            glm::perspective(glm::radians(camera.zoom), (float)props.Width / (float)props.Height, camera.nearClip, camera.farClip);
    }

    UploadUniformBuffer(camera.uniformBuffer);
}

void ProcessCameraMouse(Camera& camera, float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= camera.movementSettings.sensitivity;
    yoffset *= camera.movementSettings.sensitivity;

    camera.yaw = glm::mod(camera.yaw + xoffset, 360.0f);
    camera.pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (camera.pitch > 89.0f)
        {
            camera.pitch = 89.0f;
        }
        if (camera.pitch < -89.0f)
        {
            camera.pitch = -89.0f;
        }
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors(camera);
}

void UpdateCameraVectors(Camera& camera)
{
    // calculate the new Front vector
    glm::vec3 front;

    front.x      = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y      = sin(glm::radians(camera.pitch));
    front.z      = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front = glm::normalize(front);

    camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
    camera.up    = glm::normalize(glm::cross(camera.right, camera.front));
}
void ProcessCameraKeyboard(Camera& camera, const Window& window)
{
    GLFWwindow* nativeWindow = window.GetWindowPtr();
    float       speed        = camera.movementSettings.speed * window.GetDeltaTime();
    if (glfwGetKey(nativeWindow, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += speed * camera.front;
    if (glfwGetKey(nativeWindow, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= speed * camera.front;
    if (glfwGetKey(nativeWindow, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * speed;
    if (glfwGetKey(nativeWindow, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * speed;
    if (glfwGetKey(nativeWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.position += speed * camera.up;
    if (glfwGetKey(nativeWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.position -= speed * camera.up;
}

void SetCameraVectors(Camera& camera, const glm::vec3& position, float pitch, float yaw)
{
    camera.position = position;
    camera.yaw      = yaw;
    camera.pitch    = pitch;

    UpdateCameraVectors(camera);
}