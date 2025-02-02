#version 450
#define LIGHT_NUM 256
//layout (set = 0, binding = 3) uniform sampler2D depthSampler; //single sample
layout (set = 0, binding = 3) uniform sampler2DMS depthSampler; //msaa

layout (location = 0) in vec2 inTexCoord;
layout (location = 0) out vec4 outColor;

void main() {
	//float depthValue = texture(depthSampler, inTexCoord).r; //single sample

	vec4 color = texelFetch(depthSampler, ivec2(inTexCoord * textureSize(depthSampler)), 7); //msaa
	float depthValue = color.r;

	depthValue = pow(depthValue, 50.0);
	outColor = vec4(vec3(depthValue), 1.0);
}