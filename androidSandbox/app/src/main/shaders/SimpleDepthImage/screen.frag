#version 450
#define LIGHT_NUM 256

// struct LightStructure{
// 	vec4 lightPos;
// 	float ambientIntensity;
// 	float diffuseIntensity;
// 	float specularIntensity;
// 	float dimmerSwitch;
// };

// layout(set = 0, binding = 1) uniform UniformCustomBufferObject { 
// 	LightStructure lights[LIGHT_NUM];
// 	vec4 cameraPos; 
// } customUBO;

//layout (set = 0, binding = 3) uniform sampler2D depthSampler; //single sample
layout (set = 0, binding = 3) uniform sampler2DMS depthSampler; //msaa
//layout (set = 1, binding = 0) uniform sampler2D texSampler;


//layout (location = 0) in vec3 inNormal;
//layout (location = 1) in vec3 inColor;//inColor is not used here	
layout (location = 0) in vec2 inTexCoord;
//layout (location = 3) in vec3 inPosWorld;

layout (location = 0) out vec4 outColor;



void main() {
	//float depthValue = texture(depthSampler, inTexCoord).r; //single sample

	vec4 color = texelFetch(depthSampler, ivec2(inTexCoord * textureSize(depthSampler)), 7); //msaa
	float depthValue = color.r;

	depthValue = pow(depthValue, 50.0);
	outColor = vec4(vec3(depthValue), 1.0);
}