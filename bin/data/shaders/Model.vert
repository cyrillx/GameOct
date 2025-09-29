#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

out vec3 fragColor;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 fragTangent;

void main()
{
	gl_Position  = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
	fragColor    = vertexColor;
	fragNormal   = mat3(normalMatrix) * vertexNormal;
	fragTexCoord = vertexTexCoord;
	fragTangent  = vertexTangent;
}