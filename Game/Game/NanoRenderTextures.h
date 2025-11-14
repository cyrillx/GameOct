#pragma once

#include "NanoOpenGL3Advance.h"

struct Texture2D final
{
	GLuint   id{ 0 };
	uint32_t width{ 0 };
	uint32_t height{ 0 };
};

namespace textures
{
	bool Init();
	void Close();

	Texture2D GetDefaultDiffuse2D();
	Texture2D GetDefaultNormal2D();
	Texture2D GetDefaultSpecular2D();
	Texture2D LoadTexture2D(const std::string& fileName, ColorSpace colorSpace = ColorSpace::Linear, bool flipVertical = false);
	Texture2D CreateTextureFromData(std::string_view name, aiTexture* embTex, ColorSpace colorSpace = ColorSpace::Linear, bool flipVertical = false);
} // namespace textures