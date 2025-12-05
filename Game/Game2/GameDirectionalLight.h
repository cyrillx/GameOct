#pragma once

#include "GameLight.h"

class GameDirectionalLight final : public GameLight
{
public:
	GameDirectionalLight(const glm::vec3& d, const glm::vec3& c, float i) : GameLight(glm::vec3(0.0, 5.0, 0.0), c, 1), m_direction(d), m_shadowTarget(glm::vec3(0.0f)) {}
	GameDirectionalLight(const std::string& na, const glm::vec3& d, const glm::vec3& c, float i) : GameLight(na, glm::vec3(0.0, 5.0, 0.0), c, 1), m_direction(d), m_shadowTarget(glm::vec3(0.0f)) {}
	GameDirectionalLight(const glm::vec3& d, const glm::vec3& c, float i, float a) : GameLight(glm::vec3(0.0, 5.0, 0.0), c, i, 1), m_direction(d), m_shadowTarget(glm::vec3(0.0f)) {}
	GameDirectionalLight(const std::string& na, const glm::vec3& d, const glm::vec3& c, float i, float a) : GameLight(na, glm::vec3(0.0, 5.0, 0.0), c, i, 1), m_direction(d), m_shadowTarget(glm::vec3(0.0f)) {}

	void SetDirection(const glm::vec3& d) { m_direction = d; }
	glm::vec3 GetDirection() const { return m_direction; }

	void SetShadowTarget(const glm::vec3& d) { m_shadowTarget = d; }
	glm::vec3 GetShadowTarget() const { return m_shadowTarget; }

	glm::mat4 GetLightTransformMatrix() final
	{
		glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, .1f, 100.0f);
		glm::mat4 lightView = glm::lookAt(GetPosition(), m_shadowTarget, glm::vec3(0.0f, 1.0f, 0.0f));
		m_viewProj = lightProj * lightView;
		return m_viewProj;
	}

private:
	glm::vec3 m_direction;
	glm::vec3 m_shadowTarget;
};