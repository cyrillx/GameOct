#include "stdafx.h"
#include "Lighto2.h"

Lighto2::Lighto2()
{
	colour = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.0f;
	diffuseIntensity = 0.0f;
}

Lighto2::Lighto2(GLuint shadowWidth, GLuint shadowHeight, GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity)
{
	colour = glm::vec3(red, green, blue);
	ambientIntensity = aIntensity;
	diffuseIntensity = dIntensity;

	shadowMap = new ShadowMap();
	shadowMap->Init(shadowWidth, shadowHeight);
}