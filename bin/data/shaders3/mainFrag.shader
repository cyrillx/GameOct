#version 330 core

in VS_OUT {
	vec3 vertColor;
	vec3 normal;
	vec2 texCoords;
} fs_in;

const float alphaClippingThreshold = 0.1;

uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTex;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec4 diffuse = vec4(fs_in.vertColor, 1.0);
	if (hasDiffuseTex) 
		diffuse = texture(diffuseTexture, fs_in.texCoords) * diffuse;
	if (diffuse.a < alphaClippingThreshold) discard;

	FragColor = diffuse;
}