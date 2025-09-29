#version 330 core

in vec3 fragColor;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec3 fragTangent;

uniform sampler2D diffuseTexture;

layout(location = 0) out vec4 outputColor;

const float alphaTestThreshold = 0.1f;

void main()
{
	vec4 albedo = texture(diffuseTexture, fragTexCoord);
	if (albedo.a <= alphaTestThreshold) discard;

	outputColor = albedo;
}