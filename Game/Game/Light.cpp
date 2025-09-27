#include "stdafx.h"
#include "Light.h"

Light::Light(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
	: m_position(pos)
	, m_ambientStrength(amb)
	, m_diffuseStrength(diff)
	, m_specularStrength(spec)
{
	m_model = glm::mat4(1.0f);
}

Light::~Light()
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_vbo);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteTextures(1, &m_icon.id);
}

DirectionalLight::DirectionalLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 dir)
	: Light(pos, amb, diff, spec)
	, m_direction(glm::normalize(dir))
	/*, m_shaderIcon("../shaders/light/directional/vertex.glsl", "../shaders/light/directional/geometry.glsl", "../shaders/light/directional/fragment.glsl")
	, m_shaderDirection("../shaders/light/directional/vertex_direction.glsl", "../shaders/light/directional/geometry_direction.glsl", "../shaders/light/directional/fragment_direction.glsl")*/
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	float data[3] = { m_position.x, m_position.y, m_position.z };
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	//m_shaderIcon.use();
	//m_shaderIcon.setInt("icon", 0);
	//m_shaderDirection.use();
	//m_shaderDirection.setVec3f("direction", m_direction);

	//m_icon = createTexture("../assets/light_icons/directional.png", TEXTURE_TYPE::DIFFUSE, true);
}

void DirectionalLight::Draw()
{
	glBindVertexArray(m_vao);

	//m_shaderIcon.use();
	//m_shaderIcon.setMatrix("model", m_model);
	//m_shaderIcon.setMatrix("view", m_view);
	//m_shaderIcon.setMatrix("proj", m_proj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_icon.id);
	glDrawArrays(GL_POINTS, 0, 1);

	/*m_shaderDirection.use();
	m_shaderDirection.setMatrix("model", m_model);
	m_shaderDirection.setMatrix("view", m_view);
	m_shaderDirection.setMatrix("proj", m_proj);
	m_shaderDirection.setVec3f("direction", glm::normalize(m_direction));
	m_shaderDirection.setVec3f("right", glm::normalize(glm::cross(m_direction, glm::vec3(0.0f, 1.0f, 0.0f))));
	m_shaderDirection.setFloat("boxDim", -1.0f);*/
	// wireframe on
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.5f);
	glDrawArrays(GL_POINTS, 0, 1);
	// wireframe off
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(0);
}

void DirectionalLight::Draw(float orthoDim)
{
	glBindVertexArray(m_vao);

	//m_shaderIcon.use();
	//m_shaderIcon.setMatrix("model", m_model);
	//m_shaderIcon.setMatrix("view", m_view);
	//m_shaderIcon.setMatrix("proj", m_proj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_icon.id);
	glDrawArrays(GL_POINTS, 0, 1);

	/*m_shaderDirection.use();
	m_shaderDirection.setMatrix("model", m_model);
	m_shaderDirection.setMatrix("view", m_view);
	m_shaderDirection.setMatrix("proj", m_proj);
	m_shaderDirection.setVec3f("direction", m_direction);
	m_shaderDirection.setVec3f("right", glm::normalize(glm::cross(m_direction, glm::vec3(0.0f, 1.0f, 0.0f))));
	m_shaderDirection.setFloat("boxDim", orthoDim);*/
	// wireframe on
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.5f);
	glDrawArrays(GL_POINTS, 0, 1);
	// wireframe off
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(0);
}

LightType DirectionalLight::GetType()
{
	return LightType::Directional;
}

glm::vec3 DirectionalLight::GetDirection()
{
	return m_direction;
}

void DirectionalLight::SetDirection(glm::vec3 dir)
{
	m_direction = glm::normalize(dir);
}

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 dir, float innerAngle, float outerAngle)
	: Light(pos, amb, diff, spec)
	, m_direction(dir)
	, m_cutOff(glm::radians(innerAngle))
	, m_outerCutOff(glm::radians(outerAngle))
	//, m_shaderIcon("../shaders/light/spot/vertex.glsl", "../shaders/light/spot/geometry.glsl", "../shaders/light/spot/fragment.glsl")
	//, m_shaderCutOff("../shaders/light/spot/vertex_cutoff.glsl", "../shaders/light/spot/geometry_cutoff.glsl", "../shaders/light/spot/fragment_cutoff.glsl")
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	float data[3] = { m_position.x, m_position.y, m_position.z };
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	/*m_shaderIcon.use();
	m_shaderIcon.setInt("icon", 0);
	m_shaderCutOff.use();
	m_shaderCutOff.setVec3f("direction", m_direction);
	m_shaderCutOff.setFloat("cutOff", m_cutOff);

	m_icon = createTexture("../assets/light_icons/spot.png", TEXTURE_TYPE::DIFFUSE, true);*/
}

void SpotLight::Draw()
{
	glBindVertexArray(m_vao);
	//m_shaderIcon.use();
	//m_shaderIcon.setMatrix("model", m_model);
	//m_shaderIcon.setMatrix("view", m_view);
	//m_shaderIcon.setMatrix("proj", m_proj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_icon.id);
	glDrawArrays(GL_POINTS, 0, 1);

	/*m_shaderCutOff.use();
	m_shaderCutOff.setMatrix("model", m_model);
	m_shaderCutOff.setMatrix("view", m_view);
	m_shaderCutOff.setMatrix("proj", m_proj);
	m_shaderCutOff.setVec3f("direction", glm::normalize(m_direction));
	m_shaderCutOff.setVec3f("right", glm::normalize(glm::cross(m_direction, glm::vec3(0.0f, 1.0f, 0.0f))));
	m_shaderCutOff.setFloat("cutOff", m_cutOff);*/
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

LightType SpotLight::GetType()
{
	return LightType::Spot;
}

glm::vec3 SpotLight::GetDirection()
{
	return m_direction;
}

void SpotLight::SetDirection(glm::vec3 dir)
{
	m_direction = glm::normalize(dir);
}

float SpotLight::GetCutOff()
{
	return m_cutOff;
}

void SpotLight::SetCutOff(float cutoff)
{
	m_cutOff = glm::radians(cutoff);
}

float SpotLight::GetOuterCutOff()
{
	return m_outerCutOff;
}

void SpotLight::SetOuterCutOff(float out)
{
	m_outerCutOff = glm::radians(out);
}

glm::vec3 Light::GetPosition()
{
	return m_model * glm::vec4(m_position, 1.0f);
}

void Light::SetPosition(glm::vec3 pos)
{
	m_model = glm::translate(glm::mat4(1.0f), pos - m_position);
}

glm::vec3 Light::GetAmbientStrength()
{
	return m_ambientStrength;
}

glm::vec3 Light::GetDiffuseStrength()
{
	return m_diffuseStrength;
}

glm::vec3 Light::GetSpecularStrength()
{
	return m_specularStrength;
}

void Light::SetAmbientStrength(glm::vec3 c)
{
	m_ambientStrength = c;
}

void Light::SetDiffuseStrength(glm::vec3 c)
{
	m_diffuseStrength = c;
}

void Light::SetSpecularStrength(glm::vec3 c)
{
	m_specularStrength = c;
}

void Light::SetModelMatrix(glm::mat4 m)
{
	m_model = m;
	m_position = glm::vec3(m * glm::vec4(m_position, 1.0f));
}

void Light::SetViewMatrix(glm::mat4 m)
{
	m_view = m;
}

void Light::SetProjMatrix(glm::mat4 m)
{
	m_proj = m;
}