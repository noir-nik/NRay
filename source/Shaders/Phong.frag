#version 450
#extension GL_GOOGLE_include_directive : enable
#include "Phong.cppm"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform _constants {
	PhongConstants ctx;
};


vec3 normals(vec3 pos) {
vec3 fdx = dFdx(pos);
vec3 fdy = dFdy(pos);
return normalize(cross(fdx, fdy));
}

float attenuation(float r, float f, float d) {
float denom = d / r + 1.0;
float attenuation = 1.0 / (denom*denom);
float t = (attenuation - f) / (1.0 - f);
return max(t, 0.0);
}

#define PI 3.14159265

float computeDiffuse(
vec3 lightDirection,
vec3 viewDirection,
vec3 surfaceNormal,
float roughness,
float albedo) {

float LdotV = dot(lightDirection, viewDirection);
float NdotL = dot(lightDirection, surfaceNormal);
float NdotV = dot(surfaceNormal, viewDirection);

float s = LdotV - NdotL * NdotV;
float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

float sigma2 = roughness * roughness;
float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
float B = 0.45 * sigma2 / (sigma2 + 0.09);

return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}

void main() {
	
	// Normalize the normal vector
	vec3 normal = normalize(inNormal);

	// Light direction
	vec3 lightDir = normalize(ctx.light.position - inPosition);

	// View direction
	vec3 cameraPosition = ctx.cameraPosition;
	vec3 viewDir = normalize(cameraPosition - inPosition);

	// Reflection vector
	vec3 reflectDir = reflect(-lightDir, normal);

	// Ambient term
	vec3 ambient = ctx.material.ambient * ctx.light.color;

	// Diffuse term (Lambert)
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * ctx.material.diffuse * ctx.light.color;

	// Specular term (Phong)
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), ctx.material.shininess);
	vec3 specular = spec * ctx.material.specular * ctx.light.color;

	float cosViewNormal = dot(viewDir, normal);

	// float fresnel = clamp(1.0 + pow(1.0 - cosViewNormal, 20.0), 0.0, 1.0);
	float fresnel = -cosViewNormal * cosViewNormal + 1.0;
	fresnel = clamp(pow(1 - fresnel, 0.2), 0.0, 1.0);
	fresnel = 1.;
	// Combine
	vec3 phong = (ambient + diffuse + specular) * fresnel;
	phong = vec3(inUV, 0.0);

	// Gamma correction
	phong = pow(phong, vec3(0.454545));
	outColor = vec4(phong, 1.0); 
}