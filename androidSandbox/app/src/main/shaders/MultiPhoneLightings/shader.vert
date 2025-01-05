#version 450 core

layout(set = 0, binding = 0) uniform UniformCustomBufferObject { 
	//vec3 cameraPos;
    vec3 lightPos;
} customUBO;

layout(set = 0, binding = 1) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outViewVec;
layout (location = 3) out vec3 outLightVec;
layout (location = 4) out vec2 outTexCoord;

void main() 
{
	vec4 pos_view = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	vec4 pos_world = ubo.model * vec4(inPosition, 1.0);
	
	gl_Position = pos_view;
	outNormal = mat3(ubo.model) * inNormal;
	outColor = inColor;
	//outViewVec = customUBO.cameraPos - pos_world.xyz;	
	outViewVec = vec3(0,3,-10) - pos_world.xyz;	
	outLightVec = customUBO.lightPos - pos_world.xyz;
	outTexCoord = inTexCoord;
}