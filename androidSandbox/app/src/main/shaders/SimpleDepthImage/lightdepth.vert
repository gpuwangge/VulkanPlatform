#version 450

layout(set = 0, binding = 1) uniform UniformBufferObject {
	mat4 model;
	mat4 mainCameraProj;
	mat4 lightCameraProj;
	mat4 mainCameraView;
	mat4 lightCameraView;
	mat4 padding0;
	mat4 padding1;
	mat4 padding2; 
} mvpUBO;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor; //not used
layout(location = 2) in vec2 inTexCoord; //not used
layout(location = 3) in vec3 inNormal; //not used

void main() {
	gl_Position = mvpUBO.mainCameraProj * mvpUBO.lightCameraView * mvpUBO.model * vec4(inPosition, 1.0); //use light camera view to generate light depth image
}