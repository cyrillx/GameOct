#pragma once

#include "GameLight.h"

class GamePointLight final : public GameLight
{
public:
	GamePointLight(const glm::vec3& p, const glm::vec3& c) : GameLight(p, c, 0), m_areaOfInfluence(10.0f) {}
	GamePointLight(const glm::vec3& p, const glm::vec3& c, float a) : GameLight(p, c, 0), m_areaOfInfluence(a) {}
	GamePointLight(const std::string& na, const glm::vec3& p, const glm::vec3& c, float a) : GameLight(na, p, c, 0), m_areaOfInfluence(a) {}
	GamePointLight(const glm::vec3& p, const glm::vec3& c, float i, float a) : GameLight(p, c, i, 0), m_areaOfInfluence(a) {}
	GamePointLight(const std::string& na, const glm::vec3& p, const glm::vec3& c, float i, float a) : GameLight(na, p, c, i, 0), m_areaOfInfluence(a) {}

	void SetAreaOfInfluence(float a) { m_areaOfInfluence = a; }
	float GetAreaOfInfluence() { return m_areaOfInfluence; }

	glm::mat4 GetLightTransformMatrix() final { return m_viewProj; }

private:
	float m_areaOfInfluence; //In units
};