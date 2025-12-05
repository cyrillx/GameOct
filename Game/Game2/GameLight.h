#pragma once

#include "SceneObject.h"

class GameLight : public SceneObject
{
public:
	GameLight(const glm::vec3& p, const glm::vec3& c, const unsigned int t) :SceneObject(p, ObjectType::Light), m_color(c), m_intensity(1.0f), m_type(t), m_shadowTexture(nullptr), m_viewProj(glm::mat4(0.0f)), m_castShadows(true) {}

	GameLight(const std::string& na, const glm::vec3& p, const glm::vec3& c, const unsigned int t) :SceneObject(na, p, ObjectType::Light), m_color(c), m_intensity(1.0f), m_type(t), m_shadowTexture(nullptr), m_viewProj(glm::mat4(0.0f)), m_castShadows(true) {}

	GameLight(glm::vec3 p, glm::vec3  c, float i, const unsigned int t) :SceneObject(p, ObjectType::Light), m_color(c), m_intensity(i), m_type(t), m_shadowTexture(nullptr), m_viewProj(glm::mat4(0.0f)), m_castShadows(true) {}

	GameLight(const std::string& na, glm::vec3 p, glm::vec3  c, float i, const unsigned int t) :SceneObject(na, p, ObjectType::Light), m_color(c), m_intensity(i), m_type(t), m_shadowTexture(nullptr), m_viewProj(glm::mat4(0.0f)), m_castShadows(true) {}

	virtual void SetColor(const glm::vec3& c) { m_color = c; }
	virtual glm::vec3 GetColor() const { return m_color; }

	virtual void SetIntensity(float i) { m_intensity = i; }
	virtual float GetIntensity() const { return m_intensity; }

	virtual void SetCastShadows(bool c) { m_castShadows = c; }
	virtual bool GetCastShadows() const { return m_castShadows; }

	virtual Texture2D* GetShadowText() const { return m_shadowTexture; }

	virtual void SetShadowText(Texture2D* t)
	{
		m_shadowTexture = t;
	}

	const unsigned int GetType() const { return m_type; }

	virtual glm::mat4 GetLightTransformMatrix() = 0;

protected:
	const unsigned int m_type;
	glm::vec3          m_color;
	float              m_intensity;
	Texture2D*         m_shadowTexture;
	glm::mat4          m_viewProj;
	bool               m_castShadows;
};