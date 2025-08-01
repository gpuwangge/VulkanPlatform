#version 450
#define LIGHT_MAX 64

struct LightStructure{
	vec4 lightPos;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float dimmerSwitch;
};

layout(set = 0, binding = 0) uniform UniformLightsBufferObject { 
	LightStructure lights[LIGHT_MAX];
	vec4 mainCameraPos; 
	int lightNum; //number of lights, max is LIGHT_MAX
} lightsUBO;

// layout(set = 0, binding = 2) uniform UniformBufferObject {
// 	mat4 model;
// 	mat4 mainCameraProj;
// 	mat4 lightCameraProj;
// 	mat4 mainCameraView;
// 	mat4 lightCameraView;
// 	mat4 padding0;
// 	mat4 padding1;
// 	mat4 padding2; 
// } mvpUBO;

//layout (set = 0, binding = 3) uniform sampler2D depthSampler;//single sampled
layout (set = 0, binding = 2) uniform sampler2DMS depthSampler; //msaa, there is no use to this uniform in this shader
//layout (set = 0, binding = 4) uniform sampler2DMS lightDepthSampler; //msaa
layout (set = 0, binding = 3) uniform sampler2DShadow lightDepthSampler; //depthbias hardware compare: has bug

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
bool enablePCF = true;


float GetShadow(vec3 shadowCoords, float shadowContribution){
	//Note:The texture() here will run: return (compareDepth <= depthMap[uv]) ? 1.0 : 0.0;
	return (1.0f - texture(lightDepthSampler, vec3(shadowCoords.xy, shadowCoords.z))) * shadowContribution;
}

float PCFShadow(vec3 shadowCoords){ //Percentage Closer Filtering, shadowCoords are within 0~1, shadowCoords.xy is light space coords, shadowCoords.z is light space depth
	float shadow = 0.0f;

	ivec2 texDim = ivec2(800,800);//textureSize(lightDepthSampler);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	int range = 3;
	//float shadow_contribution = 0.23 / (range * range);
	//float shadow_contribution = 1.0 / pow((2 * range + 1), 2);
	int validSamples = 0; //use valid sampler instead of shadow_contribution, because the shadow_contribution is not accurate when the shadowCoords are near the edge of the texture
	for (int x = -range; x <= range; x++){
		for (int y = -range; y <= range; y++){
			vec2 offset = vec2(dx * x, dy * y);
			vec2 shadowCoords_offset = shadowCoords.xy+offset;
			// if(shadowCoords_offset.x >= 0.0 && shadowCoords_offset.x <= 1.0 &&
			// 	shadowCoords_offset.y >= 0.0 && shadowCoords_offset.y <= 1.0){//make sure shadowCoords_offset are still within 0~1, otherwise there is black box
			// 	float shadow_delta = GetShadow(vec3(shadowCoords_offset, shadowCoords.z-bias));
			// 	shadow += shadow_delta * shadow_contribution;
			// }
			if (all(greaterThanEqual(shadowCoords_offset, vec2(0.0))) && all(lessThanEqual(shadowCoords_offset, vec2(1.0)))) {
                float shadow_delta = GetShadow(vec3(shadowCoords_offset, shadowCoords.z), 0.95f);
                shadow += shadow_delta;// * shadow_contribution;
				validSamples += 1;
            }
		}
	}
	if (validSamples > 0) shadow /= float(validSamples); 
	else shadow = 1.0;

	return shadow;
}

void main() {
	//Code to generate light shading
	vec4 tex = texture(texSampler, inTexCoord);
	//color = vec3(mix(tex.xyz, vec3(dot(vec3(0.2126,0.7152,0.0722), tex.xyz)), 0.65));	//Desaturate tex color
	vec3 N = normalize(inNormal);

	outColor = vec4(0,0,0,0);
	for(int i = 0; i < lightsUBO.lightNum; i++){
		vec3 viewVec = lightsUBO.mainCameraPos.xyz - inPosWorld;		
		vec3 lightVec = lightsUBO.lights[i].lightPos.xyz - inPosWorld;
		float ambientIntensity = lightsUBO.lights[i].ambientIntensity * lightsUBO.lights[i].dimmerSwitch;
		float diffuseIntensity = lightsUBO.lights[i].diffuseIntensity * lightsUBO.lights[i].dimmerSwitch;
		float specularIntensity = lightsUBO.lights[i].specularIntensity * lightsUBO.lights[i].dimmerSwitch;

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
			lightSpaceCoords.xy = lightSpaceCoords.xy * 0.5 + 0.5;

			if(enablePCF) shadow = PCFShadow(lightSpaceCoords); //PCFShadow(lightSpaceCoords, inNormal, L);
			else shadow = GetShadow(lightSpaceCoords, 0.95f);
				
			// float z = lightSpaceCoords.z; //to visualize the depth
			// if (z < 0.1) outColor = vec4(0.0, 0.0, 1.0, 1.0); // blue
			// else if (z < 0.3) outColor = vec4(0.0, 1.0, 1.0, 1.0); // cyan
			// else if (z < 0.5) outColor = vec4(0.0, 1.0, 0.0, 1.0); // green
			// else if (z < 0.7) outColor = vec4(1.0, 1.0, 0.0, 1.0); // yellow
			// else if (z < 0.9) outColor = vec4(1.0, 0.5, 0.0, 1.0); // orange
			// else outColor = vec4(1.0, 0.0, 0.0, 1.0); // red

			outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0) * (1.0 - shadow);
		}else outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0);	
	}
	
}

