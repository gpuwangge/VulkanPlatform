#version 450

layout(set = 0, binding = 2) uniform UniformBufferObject {
    mat4 model;
    mat4 proj;
	mat4 mainCameraView;
	mat4 lightCameraView;
} mvpUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout (location = 0) out vec2 outTexCoord;


void main() {
	gl_Position = mvpUBO.proj * mvpUBO.mainCameraView * mvpUBO.model * vec4(inPosition, 1.0);
	outTexCoord = inTexCoord;
}