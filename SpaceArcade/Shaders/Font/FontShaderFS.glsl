#version 430 core

in vec2 TexCoords;
out vec4 color;

layout(origin_upper_left) in vec4 gl_FragCoord;

uniform sampler2D text;
uniform vec3 textColor;
uniform bool useClipSpace;
uniform vec4 clipSpace;

void main()
{
	if (useClipSpace)
	{
		if (gl_FragCoord.x < clipSpace.x || gl_FragCoord.y < clipSpace.y ||
			gl_FragCoord.x > clipSpace.z || gl_FragCoord.y > clipSpace.w)
			discard;
	}

	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
	color = vec4(textColor, 1.0) * sampled;
}