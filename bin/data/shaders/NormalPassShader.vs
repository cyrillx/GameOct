#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec4 vCol;
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 DirectionalLightSpacePos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 directionalLightTransform;

void main()
{
	vs_out.vCol = vec4(clamp(aPosition, 0.0f, 1.0f), 1.0f);

	vs_out.FragPos = (model * vec4(aPosition, 1.0)).xyz; 
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	vs_out.TexCoords = aTexCoords;
	vs_out.DirectionalLightSpacePos = directionalLightTransform * model * vec4(aPosition, 1.0);
	gl_Position = projection * view * model * vec4(aPosition, 1.0);
}