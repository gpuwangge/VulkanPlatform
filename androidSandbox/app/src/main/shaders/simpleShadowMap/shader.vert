#version 450

layout(set = 0, binding = 0) uniform UniformCustomBufferObject { 
    vec3 lightPos;
	mat4 lightSpace;
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
layout (location = 4) out vec2 fragTexCoord;
layout (location = 5) out vec4 outShadowCoord;

//biasMat is to prevent shadwo acne
const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

void main() 
{
	outNormal = inNormal;
	outColor = inColor;
	fragTexCoord = inTexCoord;
	
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	
	vec4 pos = ubo.model * vec4(inPosition, 1.0);
	outNormal = mat3(ubo.model) * inNormal;
	//vec3 lPos = mat3(ubo.model) * customUBO.lightPos;
	outLightVec = normalize(customUBO.lightPos - inPosition.xyz);
	outViewVec = -pos.xyz;		

	outShadowCoord = (biasMat * customUBO.lightSpace * ubo.model ) * vec4(inPosition, 1.0);
}