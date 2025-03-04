#version 450
#define LIGHT_NUM 256

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
layout (set = 0, binding = 4) uniform sampler2DMS lightDepthSampler; //msaa
layout (set = 1, binding = 0) uniform sampler2D texSampler;


layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;//inColor is not used here	
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inPosWorld;

layout (location = 4) in vec4 inFragPosLightSpace;

layout (location = 0) out vec4 outColor;

#define AMBIENT 0.1

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( texSampler, shadowCoord.st + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) 
		{
			shadow = AMBIENT;
		}
	}
	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(texSampler, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}

void main() {
	//1.Code to generate shadow
	float shadow = 0.0f;
	vec3 lightSpaceCoords = inFragPosLightSpace.xyz/inFragPosLightSpace.w;
	//vec3 lightSpaceCoords = inFragPosLightSpace.xyz;
	if(lightSpaceCoords.x >= -1.0 && lightSpaceCoords.x <= 1.0 && //make sure after convert to light space, the point is in view frustum; outside of view frustum has no shadow calculation
	   lightSpaceCoords.y >= -1.0 && lightSpaceCoords.y <= 1.0 &&
	   lightSpaceCoords.z >= 0.0 && lightSpaceCoords.z <= 1.0){
		lightSpaceCoords = lightSpaceCoords * 0.5 + 0.5;

		float depth_lightDepthImage = texelFetch(lightDepthSampler, ivec2(lightSpaceCoords.xy * textureSize(lightDepthSampler)), 0).r; //r==g==b, value is z
		float depth_lightSpace = lightSpaceCoords.z; //if depth_lightSpace is greater than depth_lightDepthImage, means this fragment is in shadow

		float threshold = 0.05f; //TODO: how to find this value?
		shadow = (depth_lightSpace-depth_lightDepthImage) > threshold ? 0.9 : 0.0; //currentDepth >= closestDepth

		//outColor = vec4(lightSpaceCoords.xy, 0.0, 1.0);
		//outColor = vec4(lightSpaceCoords.z, 0.0, 0.0, 1.0);
		//outColor = vec4(abs(inFragPosLightSpace.z/inFragPosLightSpace.w), 0.0, 0.0, 1.0);
	}
	//outColor = vec4(1.0-shadow, 1.0-shadow, 1.0-shadow, 1.0);

	//2.Code to generate light shading
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

		outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0) * (1.0 - shadow);
	}	
	
	//legacy code
	// vec4 tex = texture(texSampler, fragTexCoord);
	// vec3 color = tex.xyz;

	// //float shadow = (enablePCF == 1) ? filterPCF(inShadowCoord / inShadowCoord.w) : textureProj(inShadowCoord / inShadowCoord.w, vec2(0.0));
	// //float shadow = filterPCF(inShadowCoord / inShadowCoord.w);
	// float shadow = textureProj(inShadowCoord / inShadowCoord.w, vec2(0.0));

	// vec3 N = normalize(inNormal);
	// vec3 L = normalize(inLightVec);
	// vec3 V = normalize(inViewVec);
	// vec3 R = normalize(-reflect(L, N));
	// vec3 diffuse = max(dot(N, L), AMBIENT) * color;

	// outFragColor = vec4(diffuse * shadow, 1.0);
}