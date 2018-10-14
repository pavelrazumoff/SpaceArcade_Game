#version 430 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0.
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 aInstanceMatrix;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform bool useInstances;
uniform bool useShadowMapping;
uniform vec3 viewPos;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
	mat3 TBN;
} vs_out;

void main()
{
	// note that we read the multiplication from right to left
	mat4 worldMat;
	if (!useInstances)
		worldMat = model;
	else
		worldMat = aInstanceMatrix;

	gl_Position = projection * view * worldMat * vec4(aPos, 1.0);
	vs_out.Normal = transpose(inverse(mat3(worldMat))) * aNormal;
	vs_out.FragPos = vec3(worldMat * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;

	if(useShadowMapping)
		vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	
	vec3 T = normalize(vec3(worldMat * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(worldMat * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(worldMat * vec4(aNormal, 0.0)));

	// TBN must form a right handed coord system.
    // Some models have symetric UVs. Check and fix.
    if (dot(cross(N, T), B) < 0.0)
		T = T * -1.0;

    vs_out.TBN = mat3(T, B, N);
}