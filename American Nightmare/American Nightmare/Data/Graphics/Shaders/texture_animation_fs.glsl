#version 440 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specularExponent;
};

struct PointLight
{
	vec4 position;
	vec4 diffuse;
	vec4 specular;

	float strength;
	float constant;
	float linear;
	float quadratic;
	float radius;
};

struct DirectionalLight
{
	vec4 direction;

	vec4 diffuse;
	vec4 specular;

	float strength;
};

struct LightSpace
{
	mat4 matrix;
	vec4 direction;
};

struct PointShadow
{
	vec4 position;
	float farPlane;
};

// Uniform

// Lights
uniform int nrOfPointLights;
uniform PointLight pointLights[100];

uniform int nrOfDirectionalLights;
uniform DirectionalLight directionalLights[10];

// Shadows

uniform int nrOfDirectionalShadowMaps;
uniform LightSpace lightSpace[5];
uniform sampler2D dirShadowMaps[5];

uniform int nrOfPointShadowMaps;
uniform PointShadow pointShadows[5];
uniform samplerCube pointShadowMaps[5];

uniform vec4 viewPos;

uniform sampler2D texture;
uniform sampler2D normal;
uniform Material material;

uniform vec4 lightDirection;
uniform bool useShadow;
uniform bool isRight;

// Input
in vec2 textureUV;
in vec3 position;

// Output
layout(location = 0) out vec4 fragment_color;

float calculateDirShadow(vec3 lightSpacePos, vec3 normal, int shadowMapIndex)
{
	float bias = max(0.01 * (1.0 - dot(normal, lightSpace[shadowMapIndex].direction.xyz)), 0.005);
  
	float currentDepth = lightSpacePos.z;
	//float closestDepth = texture2D(dirShadowMaps[shadowMapIndex], lightSpacePos.xy).r; 
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	// PCF
	float shadow = 0.f;
	vec2 texelSize = 1.0 / textureSize(dirShadowMaps[shadowMapIndex], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture2D(dirShadowMaps[shadowMapIndex], lightSpacePos.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

	if(lightSpacePos.z > 1.0)
		shadow = 0.f;

	return shadow;
}

float calculatePointShadow(vec3 fragPos, int shadowMapIndex)
{
    vec3 fragToLight = fragPos - pointShadows[shadowMapIndex].position.xyz;
	 
    float closestDepth = textureCube(pointShadowMaps[shadowMapIndex], fragToLight).r;

    closestDepth *= pointShadows[shadowMapIndex].farPlane;
    
    float currentDepth = length(fragToLight);
    
    float bias = 0.05;
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec4 pointLightCalc(vec4 lightPosition, vec4 lightDiffuse, vec4 lightSpecular, float strength, float lightConstant, float lightLinear, float lightQuadratic, vec3 inFragPos, vec3 inNormal, vec4 inTex, float distanceToLight, float shadow)
{
	vec3 normal = normalize(inNormal);

	vec3 lightDir = normalize(lightPosition.xyz - inFragPos);
	vec4 diffuseLight = lightDiffuse * vec4(material.diffuse, 1.f) * inTex * max(dot(normal, lightDir), 0.f);

	vec3 viewDir = normalize(viewPos.xyz - inFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float specular = pow(max(dot(viewDir, reflectDir), 0.f), material.specularExponent * 1000.f);
	vec4 specularLight = specular * vec4(material.specular, 1.f) * lightSpecular;

	float attenuation = 1.0 / (1.0 + lightLinear * distanceToLight + lightQuadratic * distanceToLight * distanceToLight);

	return strength * (1.f - shadow) * (diffuseLight * attenuation + specularLight * attenuation);
}

vec4 directionalLightCalc(vec4 lightDirection, vec4 lightDiffuse, vec4 lightSpecular, float strength, vec3 inFragPos, vec3 inNormal, vec4 inTex, float shadow)
{
	vec3 normal = normalize(inNormal);
	vec3 lightDir = normalize(-lightDirection.xyz);

	vec4 diffuseLight = lightDiffuse * vec4(material.diffuse, 1.f) * inTex * max(dot(normal, lightDir), 0.f);

	vec3 viewDir = normalize(viewPos.xyz - inFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float specular = pow(max(dot(viewDir, reflectDir), 0.f), material.specularExponent * 1000.f);
	vec4 specularLight = specular * vec4(material.specular, 1.f) * lightSpecular;

	return strength * (1.f - shadow) * (diffuseLight + specularLight);
}

void main(void)
{

	vec4 bufferColor = texture2D(texture, vec2(textureUV.x, 1.f - textureUV.y));

	if (bufferColor.a < 0.01f)
		discard;

	vec3 bufferNormal = texture2D(normal, vec2(textureUV.x, 1.f - textureUV.y)).rgb;
	bufferNormal = normalize(bufferNormal * 2.0 - 1.0);

	if (!isRight)
		bufferNormal.x = bufferNormal.x * -1.f;

	vec4 result = vec4(0);

	result += vec4(material.ambient, 1.f) * bufferColor;

	float shadow = 0;

	if(useShadow && (nrOfDirectionalShadowMaps > 0 || nrOfPointShadowMaps > 0))
	{
		for(int i = 0; i < nrOfDirectionalShadowMaps; i++)
		{
			vec4 lightSpacePos = lightSpace[i].matrix * vec4(position, 1.f);
			vec3 finalLightSpacePos = lightSpacePos.xyz / lightSpacePos.w;
			finalLightSpacePos = finalLightSpacePos * 0.5f + 0.5f;

			shadow += calculateDirShadow(finalLightSpacePos, bufferNormal, i);
		}

		for(int i = 0; i < nrOfPointShadowMaps; i++)
		{
			shadow += calculatePointShadow(position, i);
		}

		shadow /= nrOfDirectionalShadowMaps + nrOfPointShadowMaps;

		if(shadow > 1.f)
			shadow = 1.f;
	}

	for(int i = 0; i < nrOfPointLights; i++)	
	{
		float distanceToLight = length(pointLights[i].position.xyz - position);

		if(distanceToLight < pointLights[i].radius)
				result += pointLightCalc(pointLights[i].position, pointLights[i].diffuse, pointLights[i].specular, pointLights[i].strength, pointLights[i].constant, pointLights[i].linear, pointLights[i].quadratic, position, bufferNormal, bufferColor, distanceToLight, shadow);	
	}
	
	for(int i = 0; i < nrOfDirectionalLights; i++)
	{
		result += directionalLightCalc(directionalLights[i].direction, directionalLights[i].diffuse, directionalLights[i].specular, directionalLights[i].strength, position, bufferNormal, bufferColor, shadow);
	}

	
	

	//if(result.x > bufferColor.x || result.y > bufferColor.y || result.z > bufferColor.z)
		//result = bufferColor;

	fragment_color = vec4(result.rgb, bufferColor.a);

	//fragment_color = vec4(bufferNormal, bufferColor.a);
	//fragment_color = vec4(normalw, bufferColor.a);
}