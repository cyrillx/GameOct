#pragma once

enum class AttachmentType : uint8_t
{
	Texture,
	TextureCubeMap,
	RenderBuffer
};

enum class AttachmentTarget : uint8_t
{
	Color,
	Depth,
	Stencil,
	DepthStencil
};

struct Attachment final
{
	GLuint           id;
	AttachmentType   type;
	AttachmentTarget target;
};

class Framebuffer final
{
public:
	Framebuffer(bool renderColor = true, bool multiSample = false, bool hdr = false);
	~Framebuffer();

	void AddAttachment(AttachmentType type, AttachmentTarget target, int width, int height, int insertPos = -1);
	void UpdateAttachment(AttachmentType type, AttachmentTarget target, int width, int height);

	void CreateDirectionalDepthFBO(int width, int height);
	void CreateOmnidirectionalDepthFBO(int width, int height);
	void CreateMultisampledFBO(int width, int height);
	void CreateResolveFBO(int width, int height);

	void UpdateDirectionalDepthFBO(int width, int height);
	void UpdateOmnidirectionalDepthFBO(int width, int height);
	void UpdateMultisampledFBO(int width, int height);
	void UpdateResolveFBO(int width, int height);

	std::vector<Attachment>& GetAttachments();
	
	void Bind();
	void Unbind();
	
	void BlitFramebuffer(Framebuffer& writeFBO, int width, int height);
	void BlitFramebuffer(std::unique_ptr<Framebuffer>& writeFBO, int width, int height);
	
	GLuint GetId();

private:
	void addColorTextureAttachment(int width, int height, int insertPos);
	void addDepthTextureAttachment(int width, int height, int insertPos);

	void addColorTextureCubemapAttachment(int width, int height, int insertPos);
	void addDepthTextureCubemapAttachment(int width, int height, int insertPos);

	void addColorRenderbufferAttachment(int width, int height, int insertPos);
	void addDepthRenderbufferAttachment(int width, int height, int insertPos);
	void addDepthStencilRenderbufferAttachment(int width, int height, int insertPos);

	void updateColorTextureAttachment(int width, int height, int insertPos);
	void updateDepthTextureAttachment(int width, int height, int insertPos);

	void updateColorTextureCubemapAttachment(int width, int height, int insertPos);
	void updateDepthTextureCubemapAttachment(int width, int height, int insertPos);

	void updateColorRenderbufferAttachment(int width, int height, int insertPos);
	void updateDepthRenderbufferAttachment(int width, int height, int insertPos);
	void updateDepthStencilRenderbufferAttachment(int width, int height, int insertPos);

	GLuint fbo{ 0 };
	bool renderColor;
	bool multiSample;
	bool HDR;
	std::vector<Attachment> attachment;
};