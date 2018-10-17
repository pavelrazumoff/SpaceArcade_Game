#version 430 core
layout(location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

uniform int frame;
uniform int numOfColumns;
uniform int numOfRows;

void main()
{
	if (numOfColumns == 1 && numOfRows == 1)
		TexCoords = vertex.zw;
	else
	{
		vec2 cr = vec2(numOfColumns, numOfRows);
		float stepX = 1.0 / cr.x;
		float stepY = 1.0 / cr.y;
		vec2 frameCoords = vec2(frame % numOfColumns, frame / numOfColumns);

		TexCoords = vec2((frameCoords.x + vertex.z) * stepX, (frameCoords.y + vertex.w) * stepY);
	}
	gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}