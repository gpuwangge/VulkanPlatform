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
	mat4 mainCameraProj;
	mat4 lightCameraProj;
	mat4 mainCameraView;
	mat4 lightCameraView;
	mat4 padding0;
	mat4 padding1;
	mat4 padding2; 
} mvpUBO;

//layout (set = 0, binding = 3) uniform sampler2D depthSampler;//single sampled
layout (set = 0, binding = 3) uniform sampler2DMS depthSampler; //msaa, there is no use to this uniform in this shader
//layout (set = 0, binding = 4) uniform sampler2DMS lightDepthSampler; //msaa
layout (set = 0, binding = 4) uniform sampler2DShadow lightDepthSampler; //depthbias hardware compare: has bug

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
bool enablePCF = false;


float GetShadow(vec3 shadowCoords, float depthBias){
	//The texture() here will run: return (compareDepth <= depthMap[uv]) ? 1.0 : 0.0;
	//return texture(lightDepthSampler, vec3(shadowCoords.xy, shadowCoords.z - depthBias));
	return texture(lightDepthSampler, vec3(shadowCoords.xy, shadowCoords.z));
}

float PCFShadow(vec3 shadowCoords){ //Percentage Closer Filtering, shadowCoords are within 0~1, shadowCoords.xy is light space coords, shadowCoords.z is light space depth
	float shadow = 0.0f;
	
	float depthBias = 0.35f; //How to find this value? search for "Depth Bias". This value should be related to slope, use slope-scale depth bias instead
	//float depthBias = SlopeScaleDepthBias(normal, lightDir, 10, 0.09); //not work correctly
	//float depthBias = 0; //use hardware depth bias
	
	ivec2 texDim = ivec2(800,800);//textureSize(lightDepthSampler);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	int range = 20;
	float shadow_contribution = 0.23 / (range * range);
	for (int x = -range; x <= range; x++){
		for (int y = -range; y <= range; y++){
			vec2 offset = vec2(dx * x, dy * y);
			vec2 shadowCoords_offset = shadowCoords.xy+offset;
			if(shadowCoords_offset.x >= 0.0 && shadowCoords_offset.x <= 1.0 &&
				shadowCoords_offset.y >= 0.0 && shadowCoords_offset.y <= 1.0){//make sure shadowCoords_offset are still within 0~1, otherwise there is black box
				float shadow_delta = GetShadow(vec3(shadowCoords_offset, shadowCoords.z), depthBias);
				shadow += shadow_delta * shadow_contribution;
			}
		}
	}

	return shadow;
}

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
			lightSpaceCoords.z -= 0.5f; //Hack: depth bias, this value is related to the slope of the surface, use hardware depth bias instead

			if(enablePCF) shadow = PCFShadow(lightSpaceCoords); //PCFShadow(lightSpaceCoords, inNormal, L);
			else shadow = GetShadow(lightSpaceCoords, 0.55f);
				
			// float z = lightSpaceCoords.z;
			// if (z < 0.1) outColor = vec4(0.0, 0.0, 1.0, 1.0); // blue
			// else if (z < 0.3) outColor = vec4(0.0, 1.0, 1.0, 1.0); // cyan
			// else if (z < 0.5) outColor = vec4(0.0, 1.0, 0.0, 1.0); // green
			// else if (z < 0.7) outColor = vec4(1.0, 1.0, 0.0, 1.0); // yellow
			// else if (z < 0.9) outColor = vec4(1.0, 0.5, 0.0, 1.0); // orange
			// else outColor = vec4(1.0, 0.0, 0.0, 1.0); // red

			outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0) * shadow;

			//float s = texture(lightDepthSampler, vec3(lightSpaceCoords.xy, lightSpaceCoords.z - 0.49f));
			//outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0) * s;
			

		}else //outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0);
		
		//outColor = vec4(0.0, 0.0, 0.0, 1.0); // black
			outColor = vec4(1.0, 0, 0, 1.0); // test: all red
		//outColor = vec4(lightSpaceCoords.xyz, 1.0); // test

		// float z = lightSpaceCoords.z;
		// if (z < 0.1) outColor = vec4(0.0, 0.0, 1.0, 1.0); // blue
		// else if (z < 0.3) outColor = vec4(0.0, 1.0, 1.0, 1.0); // cyan
		// else if (z < 0.51) outColor = vec4(0.0, 1.0, 0.0, 1.0); // green
		// else if (z < 0.79) outColor = vec4(1.0, 1.0, 0.0, 1.0); // yellow
		// else if (z < 0.9) outColor = vec4(1.0, 0.5, 0.0, 1.0); // orange
		// else outColor = vec4(1.0, 0.0, 0.0, 1.0); // red

		//outColor = vec4(vec3(z), 1.0); // depth in [0,1]

		//outColor = vec4(vec3(lightSpaceCoords.z), 1.0); // test

		//vec3 lightDir = normalize((mvpUBO.lightCameraView * vec4(0, 0, -1, 0)).xyz);
		//vec3 lightDir = normalize((inverse(mvpUBO.lightCameraView) * vec4(0, 0, -1, 0)).xyz);
		//outColor = vec4(abs(lightDir), 1.0);

		//vec3 lightToObj = normalize(inPosWorld - customUBO.lights[0].lightPos.xyz);
		//outColor = vec4(abs(lightToObj), 1.0);


		//outColor = vec4(vec3(mvpUBO.mainCameraProj[0].x, mvpUBO.mainCameraProj[1].y, mvpUBO.mainCameraProj[2].z), 1.0);
		//outColor = vec4(vec3(mvpUBO.lightCameraProj[0].x, mvpUBO.lightCameraProj[1].y, mvpUBO.lightCameraProj[2].z), 1.0);
		//outColor = vec4(vec3(mvpUBO.mainCameraView[0].x, mvpUBO.mainCameraView[1].y, mvpUBO.mainCameraView[2].z), 1.0);
		//outColor = vec4(vec3(mvpUBO.lightCameraView[0].x, mvpUBO.lightCameraView[1].y, mvpUBO.lightCameraView[2].z), 1.0);

	
		// vec4 lightSpace = mvpUBO.lightCameraProj * mvpUBO.lightCameraView * vec4(inPosWorld, 1.0);
		// vec3 lsc = lightSpace.xyz / lightSpace.w;
		// if(lsc.x >= -1.0 && lsc.x <= 1.0 && //make sure after convert to light space, the point is in view frustum; outside of view frustum has no shadow calculation
	    // 	lsc.y >= -1.0 && lsc.y <= 1.0 &&
	    // 	lsc.z >= 0.0 && lsc.z <= 1.0){
		// 	outColor = vec4(lsc * 0.5 + 0.5, 1.0);
		// }else{
		// 	outColor = vec4(1.0, 0.0, 0.0, 1.0); // red
		// }


				
	}
	
}

