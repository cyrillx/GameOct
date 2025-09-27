#pragma once

#include "Lighto2.h"

class PointLighto2 : public Lighto2
{
public:
	PointLighto2();
	PointLighto2(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat con, GLfloat lin, GLfloat exp);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
		GLuint diffuseIntensityLocation, GLuint positionLocation,
		GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

protected:
	glm::vec3 position;

	GLfloat constant, linear, exponent;
};