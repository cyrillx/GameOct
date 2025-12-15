#version 330 core

in vec2 TexCoords;

uniform sampler2D colorInput;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec3 result = texture(colorInput, TexCoords).rgb;
	FragColor = vec4(result, 1.0);
}