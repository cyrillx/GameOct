#pragma once

enum class ObjectType
{
	Model,
	Light,
	Camera
};

class SceneObject
{
public:
	SceneObject(const std::string na, ObjectType t) : m_name(na), m_enabled(true),
		m_child(nullptr), m_parent(nullptr), m_type(t), m_clones(0), m_selected(false)
	{
	}

	SceneObject(const std::string na, glm::vec3 p, ObjectType t) : m_name(na), m_enabled(true),
		m_child(nullptr), m_parent(nullptr), m_type(t), m_clones(0), m_selected(false)
	{
		m_transform.SetPosition(p);
	}

	SceneObject(glm::vec3 p, ObjectType t) : m_name(""), m_enabled(true),
		m_child(nullptr), m_parent(nullptr), m_type(t), m_clones(0), m_selected(false)
	{
		m_transform.SetPosition(p);
	}

	SceneObject(ObjectType t) : m_name(""), m_enabled(true),
		m_child(nullptr), m_parent(nullptr), m_type(t), m_clones(0), m_selected(false)
	{
	}

	virtual ~SceneObject()
	{
		delete m_child;
		delete m_parent;
	}

	virtual bool IsSelected() { return m_selected; }

	virtual void SetPosition(const glm::vec3& p) { m_transform.SetPosition(p); }
	virtual glm::vec3 GetPosition() const { return m_transform.GetPosition(); };

	virtual void SetRotation(const glm::vec3& p) { m_transform.SetRotation(p); }
	virtual glm::vec3 getRotation() const { return m_transform.GetRotation(); };

	virtual void SetScale(const glm::vec3 s) { m_transform.SetScale(s); }
	virtual glm::vec3 GetScale() const { return m_transform.GetScale(); }

	virtual Transform* GetTransform() { return &m_transform; }

	virtual void SetActive(const bool s) { m_enabled = s; }
	virtual bool IsActive() const { return m_enabled; }

	virtual std::string GetName() const { return m_name; }
	virtual void SetName(const std::string& s) { m_name = s; }

	virtual ObjectType GetObjectType() const { return m_type; }

	virtual void SetTransform(const Transform& t) { m_transform = t; }

protected:
	std::string  m_name;

	Transform    m_transform;

	SceneObject* m_child{ nullptr };
	SceneObject* m_parent{ nullptr };

	ObjectType   m_type;

	unsigned int m_clones{ 0 };
	bool         m_enabled{ false };
	bool         m_selected{ false };
};