#pragma once

#include "SceneObject.h"

class GameCamera : public SceneObject
{
	friend class CameraController;
public:
	GameCamera(glm::vec3 p = glm::vec3(0.0f, 1.0f, 8.0f), glm::vec3 f = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) : SceneObject(p, ObjectType::Camera), m_mouseSensitivity(0.1), m_yaw(-90.0f), m_pitch(0.0f), m_speed(0.1f), m_fov(45.0f), m_near(.1f), m_far(100.0f)
	{
		setView();
	}

	void SetFOV(float fov) { m_fov = fov; }
	float GetFOV() const { return m_fov; }

	void SetProj(int width, int height) { m_proj = glm::perspective(glm::radians(m_fov), (float)width / (float)height, m_near, m_far); }
	glm::mat4 GetProj() const { return m_proj; }
	glm::mat4 GetView() const { return m_view; }

	float GetFar() const { return m_far; }
	void SetFar(float f) { m_far = f; }
	float GetNear() const { return m_near; }
	void SetNear(float n) { m_near = n; }
	void SetSpeed(float n) { m_speed = n; }
	float GetSpeed() const { return m_speed; }

	void SetPosition(const glm::vec3& p)
	{
		SceneObject::SetPosition(p);
		updateCameraVectors(m_pitch, m_yaw);
	}

	void FocusOnTarget(const glm::vec3& t_position)
	{
		m_transform.SetForward(-glm::normalize(t_position - GetPosition()));
		setView();
	}

private:
	void setView()
	{
		m_view = glm::lookAt(GetPosition(), GetPosition() - m_transform.GetForward(), m_transform.GetUp());
	}
	void updateCameraVectors(float pitch, float yaw)
	{
		glm::vec3 direction;
		direction.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
		direction.y = sinf(glm::radians(pitch));
		direction.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
		m_transform.SetForward(-glm::normalize(direction));
		m_transform.SetRotation(acos(direction));
		setView();
	}

	float m_fov;

	glm::mat4 m_view;
	glm::mat4 m_proj;

	float m_near;
	float m_far;
	float m_yaw;
	float m_pitch;
	float m_mouseSensitivity;
	float m_zoom;
	float m_speed;
};