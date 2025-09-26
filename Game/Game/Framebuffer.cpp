#include "stdafx.h"
#include "Framebuffer.h"
#include "NanoLog.h"
// TODO: отрефакторить
// после создания текстуры вернуть бинд старой
//=============================================================================
Framebuffer::Framebuffer(bool color, bool ms, bool hdr)
	: renderColor(color)
	, multiSample(ms)
	, HDR(hdr)
{
	glGenFramebuffers(1, &fbo);
}
//=============================================================================
Framebuffer::~Framebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	for (int i{ 0 }; i < attachment.size(); ++i)
	{
		if (attachment.at(i).type == AttachmentType::Texture)
		{
			switch (attachment.at(i).target)
			{
			case AttachmentTarget::Color:
				if (multiSample)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
				break;
			case AttachmentTarget::Depth:
				if (multiSample)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				break;
			case AttachmentTarget::Stencil:
				if (multiSample)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				break;
			default:
				break;
			}
			glDeleteTextures(1, &attachment.at(i).id);
		}
		else if (attachment.at(i).type == AttachmentType::TextureCubeMap)
		{
			switch (attachment.at(i).target)
			{
			case AttachmentTarget::Color:
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
				break;
			case AttachmentTarget::Depth:
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
				break;
			default:
				break;
			}
			glDeleteTextures(1, &attachment.at(i).id);
		}
		else if (attachment.at(i).type == AttachmentType::RenderBuffer)
		{
			switch (attachment.at(i).target)
			{
			case AttachmentTarget::Color:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
				break;
			case AttachmentTarget::Depth:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
				break;
			case AttachmentTarget::Stencil:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
				break;
			case AttachmentTarget::DepthStencil:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
				break;
			default:
				break;
			}
			glDeleteRenderbuffers(1, &attachment.at(i).id);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
}
//=============================================================================
void Framebuffer::AddAttachment(AttachmentType type, AttachmentTarget target, int width, int height, int insertPos)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if (type == AttachmentType::Texture)
	{
		switch (target)
		{
		case AttachmentTarget::Color: addColorTextureAttachment(width, height, insertPos); break;
		case AttachmentTarget::Depth: addDepthTextureAttachment(width, height, insertPos); break;
		default: break;
		}
	}
	else if (type == AttachmentType::TextureCubeMap)
	{
		switch (target)
		{
		case AttachmentTarget::Color: addColorTextureCubemapAttachment(width, height, insertPos); break;
		case AttachmentTarget::Depth: addDepthTextureCubemapAttachment(width, height, insertPos); break;
		default: break;
		}
	}
	else if (type == AttachmentType::RenderBuffer)
	{
		switch (target)
		{
		case AttachmentTarget::Color: addColorRenderbufferAttachment(width, height, insertPos); break;
		case AttachmentTarget::Depth: addDepthRenderbufferAttachment(width, height, insertPos); break;
		case AttachmentTarget::DepthStencil: addDepthStencilRenderbufferAttachment(width, height, insertPos); break;
		default: break;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::UpdateAttachment(AttachmentType type, AttachmentTarget target, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	for (int i{ 0 }; i < attachment.size(); ++i)
	{
		if (attachment.at(i).type == type && attachment.at(i).target == target)
		{
			if (type == AttachmentType::Texture)
			{
				switch (target)
				{
				case AttachmentTarget::Color: updateColorTextureAttachment(width, height, i); break;
				case AttachmentTarget::Depth: updateDepthTextureAttachment(width, height, i); break;
				default: break;
				}
			}
			else if (type == AttachmentType::TextureCubeMap)
			{
				switch (target)
				{
				case AttachmentTarget::Color: updateColorTextureCubemapAttachment(width, height, i); break;
				case AttachmentTarget::Depth: updateDepthTextureCubemapAttachment(width, height, i); break;
				default: break;
				}
			}
			else if (type == AttachmentType::RenderBuffer)
			{
				switch (target)
				{
				case AttachmentTarget::Color: updateColorRenderbufferAttachment(width, height, i); break;
				case AttachmentTarget::Depth: updateDepthRenderbufferAttachment(width, height, i); break;
				case AttachmentTarget::DepthStencil: updateDepthStencilRenderbufferAttachment(width, height, i); break;
				default: break;
				}
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::CreateDirectionalDepthFBO(int width, int height)
{
	multiSample = false;
	renderColor = false;
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = AttachmentTarget::Depth;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_2D, buffer.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
		attachment.push_back(buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
//=============================================================================
void Framebuffer::CreateOmnidirectionalDepthFBO(int width, int height)
{
	multiSample = false;
	renderColor = false;
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = AttachmentTarget::Depth;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer.id);
	for (int i{ 0 }; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
		attachment.push_back(buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::CreateMultisampledFBO(int width, int height)
{
	multiSample = true;
	renderColor = true;
	struct Attachment bufferColor;
	bufferColor.type = AttachmentType::Texture;
	bufferColor.target = AttachmentTarget::Color;

	Attachment bufferDS;
	bufferDS.type = AttachmentType::RenderBuffer;
	bufferDS.target = AttachmentTarget::DepthStencil;

	glGenTextures(1, &bufferColor.id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bufferColor.id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glGenRenderbuffers(1, &bufferDS.id);
	glBindRenderbuffer(GL_RENDERBUFFER, bufferDS.id);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, bufferColor.id, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, bufferDS.id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		attachment.push_back(bufferColor);
		attachment.push_back(bufferDS);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::CreateResolveFBO(int width, int height)
{
	multiSample = false;
	renderColor = true;
	
	Attachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = AttachmentTarget::Color;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_2D, buffer.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.id, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
		attachment.push_back(buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
//=============================================================================
void Framebuffer::UpdateDirectionalDepthFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &attachment.at(0).id);
	attachment.clear();
	CreateDirectionalDepthFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::UpdateOmnidirectionalDepthFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
	glDeleteTextures(1, &attachment.at(0).id);
	attachment.clear();
	CreateOmnidirectionalDepthFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::UpdateMultisampledFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	glDeleteTextures(1, &attachment.at(0).id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &attachment.at(1).id);
	attachment.clear();
	CreateMultisampledFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::UpdateResolveFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &attachment.at(0).id);
	attachment.clear();
	CreateResolveFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
std::vector<Attachment>& Framebuffer::GetAttachments()
{
	return attachment;
}
//=============================================================================
void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
//=============================================================================
void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::BlitFramebuffer(Framebuffer& writeFBO, int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFBO.GetId());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================
void Framebuffer::BlitFramebuffer(std::unique_ptr<Framebuffer>& writeFBO, int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFBO->GetId());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================
GLuint Framebuffer::GetId()
{
	return fbo;
}
//=============================================================================
void Framebuffer::addColorTextureAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = AttachmentTarget::Color;
	GLint internalFormat = (HDR) ? GL_RGBA16F : GL_RGBA;
	GLenum type = (HDR) ? GL_FLOAT : GL_UNSIGNED_BYTE;

	if (multiSample)
	{
		glGenTextures(1, &buffer.id);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, buffer.id);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, internalFormat, width, height, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, buffer.id, 0);
	}
	else
	{
		glGenTextures(1, &buffer.id);
		glBindTexture(GL_TEXTURE_2D, buffer.id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.id, 0);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			attachment.push_back(buffer);
		else
			attachment.insert(attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void Framebuffer::addDepthTextureAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = AttachmentTarget::Depth;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_2D, buffer.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			attachment.push_back(buffer);
		else
			attachment.insert(attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void Framebuffer::addColorTextureCubemapAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = AttachmentTarget::Color;
	GLint internalFormat = (HDR) ? GL_RGBA16F : GL_RGBA;
	GLenum type = (HDR) ? GL_FLOAT : GL_UNSIGNED_BYTE;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer.id);
	for (int i{ 0 }; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, GL_RGBA, type, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer.id, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			attachment.push_back(buffer);
		else
			attachment.insert(attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void Framebuffer::addDepthTextureCubemapAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = AttachmentTarget::Depth;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer.id);
	for (int i{ 0 }; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			attachment.push_back(buffer);
		else
			attachment.insert(attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void Framebuffer::addColorRenderbufferAttachment(int width, int height, int insertPos)
{
	Attachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = AttachmentTarget::Color;
	GLint internalFormat = (HDR) ? GL_RGBA16F : GL_RGBA8;

	glGenRenderbuffers(1, &buffer.id);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.id);
	if (multiSample)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, internalFormat, width, height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffer.id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete!");
	else
	{
		if (insertPos == -1)
			attachment.push_back(buffer);
		else
			attachment.insert(attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void Framebuffer::addDepthRenderbufferAttachment(int width, int height, int insertPos)
{
	Attachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = AttachmentTarget::Depth;

	glGenRenderbuffers(1, &buffer.id);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.id);
	if (multiSample)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, width, height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			attachment.push_back(buffer);
		else
			attachment.insert(attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void Framebuffer::addDepthStencilRenderbufferAttachment(int width, int height, int insertPos)
{
	Attachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = AttachmentTarget::DepthStencil;

	glGenRenderbuffers(1, &buffer.id);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.id);
	if (multiSample)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer.id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete!");
	else
	{
		if (insertPos == -1)
			attachment.push_back(buffer);
		else
			attachment.insert(attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void Framebuffer::updateColorTextureAttachment(int width, int height, int insertPos)
{
	if (multiSample)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &attachment.at(insertPos).id);

	attachment.erase(attachment.begin() + insertPos);

	if (insertPos == attachment.size())
		addColorTextureAttachment(width, height, -1);
	else
		addColorTextureAttachment(width, height, insertPos);
}
//=============================================================================
void Framebuffer::updateDepthTextureAttachment(int width, int height, int insertPos)
{
	if (multiSample)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &attachment.at(insertPos).id);

	attachment.erase(attachment.begin() + insertPos);

	if (insertPos == attachment.size())
		addDepthTextureAttachment(width, height, -1);
	else
		addDepthTextureAttachment(width, height, insertPos);
}
//=============================================================================
void Framebuffer::updateColorTextureCubemapAttachment(int width, int height, int insertPos)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
	glDeleteTextures(1, &attachment.at(insertPos).id);

	attachment.erase(attachment.begin() + insertPos);

	if (insertPos == attachment.size())
		addColorTextureCubemapAttachment(width, height, -1);
	else
		addColorTextureCubemapAttachment(width, height, insertPos);
}
//=============================================================================
void Framebuffer::updateDepthTextureCubemapAttachment(int width, int height, int insertPos)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
	glDeleteTextures(1, &attachment.at(insertPos).id);

	attachment.erase(attachment.begin() + insertPos);

	if (insertPos == attachment.size())
		addDepthTextureCubemapAttachment(width, height, -1);
	else
		addDepthTextureCubemapAttachment(width, height, insertPos);
}
//=============================================================================
void Framebuffer::updateColorRenderbufferAttachment(int width, int height, int insertPos)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &attachment.at(insertPos).id);

	attachment.erase(attachment.begin() + insertPos);

	if (insertPos == attachment.size())
		addColorRenderbufferAttachment(width, height, -1);
	else
		addColorRenderbufferAttachment(width, height, insertPos);
}
//=============================================================================
void Framebuffer::updateDepthRenderbufferAttachment(int width, int height, int insertPos)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &attachment.at(insertPos).id);

	attachment.erase(attachment.begin() + insertPos);

	if (insertPos == attachment.size())
		addDepthRenderbufferAttachment(width, height, -1);
	else
		addDepthRenderbufferAttachment(width, height, insertPos);
}
//=============================================================================
void Framebuffer::updateDepthStencilRenderbufferAttachment(int width, int height, int insertPos)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &attachment.at(insertPos).id);

	attachment.erase(attachment.begin() + insertPos);

	if (insertPos == attachment.size())
		addDepthStencilRenderbufferAttachment(width, height, -1);
	else
		addDepthStencilRenderbufferAttachment(width, height, insertPos);
}
//=============================================================================