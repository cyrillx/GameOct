#version 330 core

uniform sampler2D frameTexture;

in vec2 fsTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = texture(frameTexture, fsTexCoord);
}