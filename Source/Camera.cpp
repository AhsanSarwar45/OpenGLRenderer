#include "Camera.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Camera* Camera::s_ActiveCamera = nullptr;

Camera::Camera(Window* window, float nearClip, float farClip, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: m_Window(window), m_NearClip(nearClip), m_FarClip(farClip), m_NativeWindow(window->GetWindowPtr()), m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_Zoom(ZOOM)
{
	m_Position = position;
	m_WorldUp = up;
	m_Yaw = yaw;
	m_Pitch = pitch;
	UpdateCameraVectors();

	s_ActiveCamera = this;

	glGenBuffers(1, &m_UniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_UniformBuffer, 0, 2 * sizeof(glm::mat4));
}
// constructor with scalar values
Camera::Camera(Window* window, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: m_Window(window), m_NativeWindow(window->GetWindowPtr()), m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_Zoom(ZOOM)
{
	
}

Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{
	if (glfwGetKey(m_NativeWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_NativeWindow, true);

	if (m_Window->IsFocused())
	{
		float speed = 2.5 * deltaTime;
		if (glfwGetKey(m_NativeWindow, GLFW_KEY_W) == GLFW_PRESS)
			m_Position += speed * m_Front;
		if (glfwGetKey(m_NativeWindow, GLFW_KEY_S) == GLFW_PRESS)
			m_Position -= speed * m_Front;
		if (glfwGetKey(m_NativeWindow, GLFW_KEY_A) == GLFW_PRESS)
			m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * speed;
		if (glfwGetKey(m_NativeWindow, GLFW_KEY_D) == GLFW_PRESS)
			m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * speed;
		if (glfwGetKey(m_NativeWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
			m_Position += speed * m_Up;
		if (glfwGetKey(m_NativeWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			m_Position -= speed * m_Up;

		ProcessMouseMovement(m_Window->GetProperties().OffsetX, m_Window->GetProperties().OffsetY);
	}

	m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	m_Projection = glm::perspective(glm::radians(m_Zoom), (float)m_Window->GetProperties().Width / (float)m_Window->GetProperties().Height, m_NearClip, m_FarClip);

	glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_Projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_View));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= m_MouseSensitivity;
	yoffset *= m_MouseSensitivity;

	m_Yaw = glm::mod(m_Yaw + xoffset, 360.0f);
	m_Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (m_Pitch > 89.0f)
		{
			m_Pitch = 89.0f;
		}
		if (m_Pitch < -89.0f)
		{
			m_Pitch = -89.0f;
		}
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(front);

	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}