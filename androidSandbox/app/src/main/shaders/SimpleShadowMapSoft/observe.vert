#version 450

layout(set = 0, binding = 1) uniform UniformBufferObject {
	mat4 model;
	mat4 mainCameraProj;
	//mat4 lightCameraProj;
	mat4 mainCameraView;
	//mat4 lightCameraView;
	mat4 padding0;
	mat4 padding1;
	mat4 padding2;
	mat4 padding3;
	mat4 padding4;
} mvpUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout (location = 0) out vec2 outTexCoord;


void main() {
	gl_Position = mvpUBO.mainCameraProj * mvpUBO.mainCameraView * mvpUBO.model * vec4(inPosition, 1.0);
	outTexCoord = inTexCoord;
}