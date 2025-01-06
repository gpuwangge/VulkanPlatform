#version 450 core
#define LIGHT_NUM 3

struct LightStructure{
	vec4 cameraPos; 
	vec4 lightPos;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float padding;
};

layout(set = 0, binding = 0) uniform UniformCustomBufferObject { 
	LightStructure lights[LIGHT_NUM];
} customUBO;

layout(set = 0, binding = 1) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvpUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outTexCoord;

layout (location = 3+0*LIGHT_NUM) out vec3 outViewVec[LIGHT_NUM];
layout (location = 3+1*LIGHT_NUM) out vec3 outLightVec[LIGHT_NUM];
layout (location = 3+2*LIGHT_NUM) out float outAmbientIntensity[LIGHT_NUM];
layout (location = 3+3*LIGHT_NUM) out float outDiffuseIntensity[LIGHT_NUM];
layout (location = 3+4*LIGHT_NUM) out float outSpecularIntensity[LIGHT_NUM];

void main() 
{
	vec4 pos_view = mvpUBO.proj * mvpUBO.view * mvpUBO.model * vec4(inPosition, 1.0);
	vec4 pos_world = mvpUBO.model * vec4(inPosition, 1.0);
	
	gl_Position = pos_view;

	outNormal = mat3(mvpUBO.model) * inNormal;
	outColor = inColor;
	outTexCoord = inTexCoord;

	for(int i = 0; i < LIGHT_NUM; i++){
		outViewVec[i] = customUBO.lights[i].cameraPos.xyz - pos_world.xyz;		
		outLightVec[i] = customUBO.lights[i].lightPos.xyz - pos_world.xyz;
		outAmbientIntensity[i] = customUBO.lights[i].ambientIntensity;
		outDiffuseIntensity[i] = customUBO.lights[i].diffuseIntensity;
		outSpecularIntensity[i] = customUBO.lights[i].specularIntensity;
	}
}