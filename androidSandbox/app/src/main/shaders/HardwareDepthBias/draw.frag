#version 450
#define LIGHT_NUM 1 //max 256

struct LightStructure{
	vec4 lightPos;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float dimmerSwitch;
};

layout(set = 0, binding = 1) uniform UniformCustomBufferObject { 
	LightStructure lights[LIGHT_NUM];
	vec4 cameraPos; 
} customUBO;

layout(set = 0, binding = 2) uniform UniformBufferObject {
    mat4 model;
    mat4 proj;
	mat4 mainCameraView;
	mat4 lightCameraView;	
} mvpUBO;

//layout (set = 0, binding = 3) uniform sampler2D depthSampler;//single sampled
layout (set = 0, binding = 3) uniform sampler2DMS depthSampler; //msaa, there is no use to this uniform in this shader
//layout (set = 0, binding = 4) uniform sampler2DMS lightDepthSampler; //msaa
layout (set = 0, binding = 4) uniform sampler2DShadow lightDepthSampler; //depthbias hardware compare
layout (set = 1, binding = 0) uniform sampler2D texSampler;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor; //inColor is not used here
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inPosWorld;

layout (location = 4) in vec4 inFragPosLightSpace;

layout (location = 0) out vec4 outColor;

//Normal Shadow: hard shadows
//PCF: fixed fiter size, soft shadow, slow
//PCSS: adaptive filter size(depends on object distance), better soft shadow, slow
//bool enablePCF = true;

void main() {

	//Code to generate light shading
	vec4 tex = texture(texSampler, inTexCoord);
	//color = vec3(mix(tex.xyz, vec3(dot(vec3(0.2126,0.7152,0.0722), tex.xyz)), 0.65));	//Desaturate tex color
	vec3 N = normalize(inNormal);

	outColor = vec4(0,0,0,0);
	for(int i = 0; i < LIGHT_NUM; i++){
		vec3 viewVec = customUBO.cameraPos.xyz - inPosWorld;		
		vec3 lightVec = customUBO.lights[i].lightPos.xyz - inPosWorld;
		float ambientIntensity = customUBO.lights[i].ambientIntensity * customUBO.lights[i].dimmerSwitch;
		float diffuseIntensity = customUBO.lights[i].diffuseIntensity * customUBO.lights[i].dimmerSwitch;
		float specularIntensity = customUBO.lights[i].specularIntensity * customUBO.lights[i].dimmerSwitch;

		vec3 L = normalize(lightVec); //point from pos to light
		vec3 V = normalize(viewVec); //point from pos to camera
		vec3 R = reflect(-L, N); //calculate from L
		float distCoff = pow(lightVec.x, 2) + pow(lightVec.y, 2) + pow(lightVec.z, 2);
		vec3 ambient = tex.xyz / distCoff;
		vec3 diffuse = max(dot(N, L), 0.0) * tex.xyz / distCoff; 
		vec3 specular = pow(max(dot(R, V), 0.0), 32.0) * vec3(0.35) / distCoff;

		//Code to generate shadow(need use L)
		float shadow = 0.0f;
		vec3 lightSpaceCoords = inFragPosLightSpace.xyz/inFragPosLightSpace.w;
		if(lightSpaceCoords.x >= -1.0 && lightSpaceCoords.x <= 1.0 && //make sure after convert to light space, the point is in view frustum; outside of view frustum has no shadow calculation
	    	lightSpaceCoords.y >= -1.0 && lightSpaceCoords.y <= 1.0 &&
	    	lightSpaceCoords.z >= 0.0 && lightSpaceCoords.z <= 1.0){
			lightSpaceCoords = lightSpaceCoords * 0.5 + 0.5;

			shadow = texture(lightDepthSampler, lightSpaceCoords);
		}

		outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0) * (1.0 - shadow);
		//outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0) * shadow;
		//outColor = vec4(vec3(shadow), 1.0);
	}	
	
}

//Notes:
//texelFetch() Explain: same function as texture , but no interpolate or filter
//	sampler
//	ivec2/ivec3: texture coordinates
//	lod: Level of Details
//	return: vec4, ivec4 or uvec4, depending on sampler type

//texture() Explain: get texel from texture
//	texture coordinate use unified values

//textureSize() Explain: get texture size
//	sampler
//	lod
//	return: texture's resolution, int, ivec2 or ivec3