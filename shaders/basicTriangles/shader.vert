#version 450

//difference of ubo and vertex buffer:
//vertex buffer: each vertex has different buffer(store different attributes)
//ubo: universal same
//If you need different attribute for each vertex, use vertex buffer; otherwise use ubo
layout(binding = 0) uniform UniformBufferObject { 
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor; //location 0,1,2... are place to store attributes for each vertex. 

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor; //color attribute of each vertex will be assigned to each fragment. If all colors are the same, this is not good. should use ubo instead
}