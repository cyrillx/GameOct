#pragma once

class ShadowMap final
{
public:
	ShadowMap();

	bool Init(unsigned int width, unsigned int height);

	void Write();

	void Read(GLenum TextureUnit);

	GLuint GetShadowWidth() { return shadowWidth; }
	GLuint GetShadowHeight() { return shadowHeight; }

	~ShadowMap();
protected:
	GLuint FBO, shadowMap;
	GLuint shadowWidth, shadowHeight;
};
