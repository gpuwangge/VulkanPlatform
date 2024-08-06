#version 450

layout (set = 1, binding = 0) uniform sampler2D texSampler;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;//inColor is not used here	
layout (location = 2) in vec3 inViewVec;
layout (location = 3) in vec3 inLightVec;
layout (location = 4) in vec2 fragTexCoord;

layout (location = 0) out vec4 outFragColor;

void main() {
	vec4 tex = texture(texSampler, fragTexCoord);
	vec3 color = tex.xyz;
	//color = vec3(mix(color, vec3(dot(vec3(0.2126,0.7152,0.0722), color)), 0.65));	//Desaturate tex color

	vec3 ambient = color * vec3(0.1f);
	vec3 N = normalize(inNormal);
	vec3 L = normalize(inLightVec);
	vec3 V = normalize(inViewVec);
	vec3 R = reflect(-L, N);
	vec3 diffuse = max(dot(N, L), 0.0) * color;
	vec3 specular = pow(max(dot(R, V), 0.0), 32.0) * vec3(0.35);
	outFragColor = vec4(ambient + diffuse * 0.5f + specular, 1.0); //vec4(ambient + diffuse * 1.75 + specular, 1.0);
}