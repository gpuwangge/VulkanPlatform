#version 450 core
#define LIGHT_NUM 256

struct LightStructure{
	vec4 lightPos;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float dimmerSwitch;
};

layout(set = 0, binding = 0) uniform UniformCustomBufferObject { 
	LightStructure lights[LIGHT_NUM];
	vec4 cameraPos; 
} customUBO;

layout (set = 1, binding = 0) uniform sampler2D texSampler;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;//inColor is not used here	
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inPosWorld;

// layout (location = 3+0*LIGHT_NUM) in vec3 inViewVec[LIGHT_NUM];
// layout (location = 3+1*LIGHT_NUM) in vec3 inLightVec[LIGHT_NUM];
// layout (location = 3+2*LIGHT_NUM) in float inAmbientIntensity[LIGHT_NUM];
// layout (location = 3+3*LIGHT_NUM) in float inDiffuseIntensity[LIGHT_NUM];
// layout (location = 3+4*LIGHT_NUM) in float inSpecularIntensity[LIGHT_NUM];

layout (location = 0) out vec4 outColor;

void main() {
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

		outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0);
	}
}