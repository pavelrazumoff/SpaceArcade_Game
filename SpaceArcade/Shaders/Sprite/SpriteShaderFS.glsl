#version 430 core
in vec2 TexCoords;

out vec4 color;

layout(origin_upper_left) in vec4 gl_FragCoord;

uniform sampler2D image;
uniform bool useImage;
uniform vec4 background_color;
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

	if (useImage)
		color = texture(image, TexCoords) * background_color;
	else
		color = background_color;
}