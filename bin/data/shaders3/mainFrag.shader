#version 330 core

struct SphereLight
{
	vec3 position;
	float radius;
	vec3 startColor;
	vec3 endColor;
	float intensity;  // Яркость света
};

in VS_OUT {
	vec3 vertColor;
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

const float alphaClippingThreshold = 0.1;

uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTex;

layout(location = 0) out vec4 FragColor;

vec3 calculateSphereLight(in SphereLight light, in vec3 point)
{
	float distance = length(point - light.position);
	float t = clamp(distance / light.radius, 0.0, 1.0);
	vec3 color = mix(light.startColor, light.endColor, t);
	float attenuation = 1.0 - t;
	return color * attenuation * light.intensity;
}

void main()
{
	vec4 diffuse = vec4(fs_in.vertColor, 1.0);
	if (hasDiffuseTex) 
		diffuse = texture(diffuseTexture, fs_in.texCoords) * diffuse;
	if (diffuse.a < alphaClippingThreshold) discard;

	SphereLight light;
	light.position = vec3(0.0, 0.0, 0.0);
	light.radius = 3.0;
	light.startColor = vec3(1.0, 0.0, 0.0);
	light.endColor = vec3(0.0, 0.0, 0.2);
	light.intensity = 5.0;

	vec3 lightResult = calculateSphereLight(light, fs_in.worldPos);

	vec3 finalColor = min(diffuse.rgb + lightResult, vec3(1.0));

	FragColor = vec4(finalColor, diffuse.a);
}