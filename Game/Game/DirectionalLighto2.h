#pragma once

#include "Lighto2.h"

class DirectionalLighto2 : public Lighto2
{
public:
	DirectionalLighto2();
	DirectionalLighto2(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation, GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	glm::mat4 CalculateLightTransform();

private:
	glm::vec3 direction;
};