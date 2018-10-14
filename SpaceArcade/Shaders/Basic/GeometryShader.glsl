#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

in VS_OUT {
    vec3 Normal;
	vec3 FragPos;
    vec2 texCoords;
} gs_in[];

void main() {
	// Basic idle geometry shader for demonstrating purposes.

	gl_Position = gl_in[0].gl_Position;
	Normal = gs_in[0].Normal;
	FragPos = gs_in[0].FragPos;
	TexCoords = gs_in[0].texCoords;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	Normal = gs_in[1].Normal;
	FragPos = gs_in[1].FragPos;
	TexCoords = gs_in[1].texCoords;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	Normal = gs_in[2].Normal;
	FragPos = gs_in[2].FragPos;
	TexCoords = gs_in[2].texCoords;
	EmitVertex();

	EndPrimitive();
}