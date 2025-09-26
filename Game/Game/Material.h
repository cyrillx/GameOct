#pragma once

class Materialo2
{
public:
	Materialo2();
	Materialo2(GLfloat sIntensity, GLfloat shine);

	void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

private:
	GLfloat specularIntensity;
	GLfloat shininess;
};