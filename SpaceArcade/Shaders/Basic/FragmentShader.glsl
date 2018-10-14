#version 430 core
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
	mat3 TBN;
} fs_in;

out vec4 FragColor;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;

	vec4 diffuse_color;
	vec4 specular_color;
	float shininess;

	bool use_texture_diffuse;
	bool use_texture_specular;
	bool use_texture_normal;
	bool use_texture_height;
};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_DIR_LIGHTS		4
#define NR_POINT_LIGHTS		4
#define NR_SPOT_LIGHTS		4

uniform DirLight dirLight[NR_DIR_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight[NR_SPOT_LIGHTS];

uniform int dirCount;
uniform int pointCount;
uniform int spotCount;

uniform Material material;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float near_plane;
uniform float far_plane;

uniform float heightScale;

uniform bool useShadowMapping;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);
float LinearizeDepth(float depth);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

vec4 mat_diff;
vec4 mat_spec;

void main()
{
	// properties
	// obtain normal from normal map in range [0,1]
    vec3 norm;
	vec3 viewDir;

	vec2 texCoords = fs_in.TexCoords;

	if(material.use_texture_normal || material.use_texture_height)
		viewDir = fs_in.TBN * normalize(viewPos - fs_in.FragPos);
	else
		viewDir = normalize(viewPos - fs_in.FragPos);

	if(material.use_texture_height)
	{
		texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);
		if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
			discard;
	}

	if(material.use_texture_normal)
	{
		norm = texture(material.texture_normal1, texCoords).rgb;
		norm = normalize(norm * 2.0 - 1.0);
	}
	else
		norm = normalize(fs_in.Normal);

	vec4 result = vec4(0.0, 0.0, 0.0, 0.0);

	mat_diff = material.diffuse_color;
	if(material.use_texture_diffuse)
		mat_diff = texture(material.texture_diffuse1, texCoords);

	mat_spec = material.specular_color;
	if(material.use_texture_specular)
		mat_spec = texture(material.texture_specular1, texCoords);

	// phase 1: Directional lighting
	int lightsCount = dirCount;
	lightsCount = min(lightsCount, NR_DIR_LIGHTS);

	for(int i = 0; i < lightsCount; i++)
		result = vec4(CalcDirLight(dirLight[i], norm, viewDir), 1.0);

	// phase 2: Point lights
	lightsCount = pointCount;
	lightsCount = min(lightsCount, NR_POINT_LIGHTS);

	for(int i = 0; i < lightsCount; i++)
		result += vec4(CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir), 1.0);

	// phase 3: Spot light
	lightsCount = spotCount;
	lightsCount = min(lightsCount, NR_SPOT_LIGHTS);

	for(int i = 0; i < lightsCount; i++)
		result += vec4(CalcSpotLight(spotLight[i], norm, fs_in.FragPos, viewDir), 1.0);

	result.a = mat_diff.a;

	if(result.a < 0.1)
		discard;

	FragColor = result;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir;
	if(material.use_texture_normal)
		lightDir = fs_in.TBN * normalize(-light.direction);
	else
		lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	// combine results
	vec3 ambient = light.ambient * vec3(mat_diff);
	vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
	vec3 specular = light.specular * spec * vec3(mat_spec);

	vec3 lighting;
	if(useShadowMapping)
	{
		float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
		lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
	}else
		lighting = (ambient + diffuse + specular);

	return lighting;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir;
	if(material.use_texture_normal)
		lightDir = fs_in.TBN * normalize(light.position - fragPos);
	else
		lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	// attenuation
	float l_distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * l_distance + light.quadratic * (l_distance * l_distance));
	//float attenuation = 1.0 / (gamma ? distance * distance : distance);

	// combine results
	vec3 ambient = light.ambient * vec3(mat_diff);
	vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
	vec3 specular = light.specular * spec * vec3(mat_spec);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 lighting;
	if(useShadowMapping)
	{
		float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
		lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
	}else
		lighting = (ambient + diffuse + specular);

	return lighting;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir;
	if(material.use_texture_normal)
		lightDir = fs_in.TBN * normalize(light.position - fragPos);
	else
		lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * vec3(mat_diff);
    vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
    vec3 specular = light.specular * spec * vec3(mat_spec);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    vec3 lighting;
	if(useShadowMapping)
	{
		float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
		lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
	}else
		lighting = (ambient + diffuse + specular);

	return lighting;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
	closestDepth = LinearizeDepth(closestDepth);
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	currentDepth = LinearizeDepth(currentDepth);
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.1 * (1.0 - dot(normal, lightDir)), 0.05);
	//bias = 0.005f;
	//float bias = 0.005 * tan(acos(dot(normal, lightDir))); // cosTheta is dot( n,l ), clamped between 0 and 1
	//bias = clamp(bias, 0.0, 0.01);

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			pcfDepth = LinearizeDepth(pcfDepth);
            shadow += (currentDepth - bias) > pcfDepth  ? 1.0 : 0.0;
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = 10;//mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = (viewDir.xy / viewDir.z) * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.texture_height1, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}