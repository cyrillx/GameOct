#pragma once

enum class ObjectType
{
	Model,
	Light,
	Camera
};

class Transform final
{
public:
	Transform(
		glm::mat4 worldMatrix = glm::mat4(1.0f),
		glm::vec3 rotation = glm::vec3(0.0f),
		glm::vec3 scale = glm::vec3(1.0f),
		glm::vec3 position = glm::vec3(0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f)

	)   : m_position(position)
		, m_scale(scale)
		, m_rotation(rotation)
		, m_up(up)
		, m_forward(forward)
		, m_right(right)
		, m_worldMatrix(worldMatrix)
		, m_isDirty(true)
	{
	}

	void SetPosition(const glm::vec3& p)
	{
		m_position = p;
		m_isDirty = true;
	}

	glm::vec3 GetPosition() const
	{
		return m_position;
	}

	void SetScale(const glm::vec3& s)
	{
		m_scale = s;
		m_isDirty = true;
	}
	glm::vec3 GetScale() const
	{
		return m_scale;
	}

	void SetRotation(const glm::vec3& r)
	{
		m_rotation = r;
		m_isDirty = true;
	}
	glm::vec3 GetRotation() const
	{
		return m_rotation;
	}

	void SetUp(const glm::vec3& u)
	{
		m_up = glm::normalize(u);
	}
	glm::vec3 GetUp() const
	{
		return m_up;
	}

	void SetForward(const glm::vec3& u)
	{
		m_forward = glm::normalize(u);
	}
	glm::vec3 GetForward() const
	{
		return m_forward;
	}

	void SetRight(const glm::vec3& u)
	{
		m_right = glm::normalize(u);
	}
	glm::vec3 GetRight() const
	{
		return m_right;
	}

	glm::mat4 GetWorldMatrix()
	{
		if (m_isDirty)
		{
			m_worldMatrix = glm::mat4(1.0f);
			m_worldMatrix = glm::translate(m_worldMatrix, m_position);
			m_worldMatrix = glm::rotate(m_worldMatrix, m_rotation.x, glm::vec3(1, 0, 0));
			m_worldMatrix = glm::rotate(m_worldMatrix, m_rotation.y, glm::vec3(0, 1, 0));
			m_worldMatrix = glm::rotate(m_worldMatrix, m_rotation.z, glm::vec3(0, 0, 1));
			m_worldMatrix = glm::scale(m_worldMatrix, m_scale);
			m_isDirty = false;
		}
		return m_worldMatrix;
	}
	void SetWorldMatrix(const glm::mat4& w)
	{
		m_worldMatrix = w;
	}
	bool GetIsDirty() const { return m_isDirty; }

private:
	glm::mat4 m_worldMatrix;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::vec3 m_forward;
	bool      m_isDirty;
};