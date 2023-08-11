#version 450

layout(binding = 0) uniform UniformCustomBufferObject { 
    vec3 lightPos;
} customUBO;

layout(binding = 1) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

// layout(location = 0) out vec3 fragColor;
// layout(location = 1) out vec2 fragTexCoord;


// layout (location = 0) in vec3 inPos;
// layout (location = 1) in vec3 inNormal;
// layout (location = 2) in vec3 inColor;

// layout (binding = 0) uniform UBO 
// {
// 	mat4 projection;
// 	mat4 model;
// 	vec4 lightPos;
// } ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outViewVec;
layout (location = 3) out vec3 outLightVec;
layout (location = 4) out vec2 fragTexCoord;

void main() 
{
	outNormal = inNormal;
	outColor = inColor;
	
	//gl_Position = ubo.proj * ubo.model * vec4(inPosition, 1.0);
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);// - vec4(0.0f,0.0f,1.0f,0.0f));
	
	vec4 pos = ubo.model * vec4(inPosition, 1.0);
	outNormal = mat3(ubo.model) * inNormal;
	//vec3 lPos = mat3(ubo.model) * vec3(5.0f, 0.0f, 0.0f); //ubo.lightPos.xyz;
	vec3 lPos = mat3(ubo.model) * customUBO.lightPos;
	outLightVec = lPos - pos.xyz;
	outViewVec = -pos.xyz;		

	fragTexCoord = inTexCoord;
}