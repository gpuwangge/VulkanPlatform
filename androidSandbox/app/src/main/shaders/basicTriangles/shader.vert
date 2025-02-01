#version 450

layout(set = 0, binding = 0) uniform UniformCustomBufferObject { 
    vec3 color;
} customUBO;

//difference of ubo and vertex buffer:
//vertex buffer: each vertex has different buffer(store different attributes)
//ubo: universal same
//If you need different attribute for each vertex, use vertex buffer; otherwise use ubo
layout(set = 0, binding = 1) uniform UniformMVPBufferObject { 
    mat4 model;
    mat4 proj;
	mat4 mainCameraView;
	mat4 lightCameraView;	    
} mvpUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor; //location 0,1,2... are place to store attributes for each vertex. 
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal; //normal is not used here


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = mvpUBO.proj * mvpUBO.mainCameraView * mvpUBO.model * vec4(inPosition, 1.0);
    fragColor = customUBO.color; //color attribute of each vertex will be assigned to each fragment. If all colors are the same, this is not good. should use ubo instead
    fragTexCoord = inTexCoord;
}