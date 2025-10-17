#pragma once

#include "Framebuffer.h"

class RPPostFrame final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(Framebuffer* preFBO);

	Framebuffer* GetFBO() const { return m_fbo.get(); }
	GLuint GetFBOId() const { return m_fbo->GetId(); }
	uint16_t GetWidth() const { return m_framebufferWidth; }
	uint16_t GetHeight() const { return m_framebufferHeight; }
private:
	std::unique_ptr<Framebuffer> m_fbo;
	GLuint                       m_program{ 0 };
	GLuint                       m_vao{ 0 };
	GLuint                       m_vbo{ 0 };
	uint16_t                     m_framebufferWidth{ 0 };
	uint16_t                     m_framebufferHeight{ 0 };
};