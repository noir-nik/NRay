#version 460
#extension GL_GOOGLE_include_directive : enable
#include "Indexing.h"
#include "Opaque.h"
#include "Pbr.h"

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec2 inUV;
layout(location = 3) in mat3 inTBN;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform _constants {
	OpaqueConstants ctx;
};

const float PI = 3.14159265359;

void main() {	
	vec4 baseColor = MATERIAL.baseColorFactor;
	if (EXISTS(MATERIAL.baseColorTexture)) {
		baseColor *= texture(textures[MATERIAL.baseColorTexture], inUV);
	}

	// Metallic Roughness
	vec2 metallicRoughness = vec2(MATERIAL.metallicFactor, MATERIAL.roughnessFactor);
	if (EXISTS(MATERIAL.metallicRoughnessTexture)) {
		metallicRoughness *= texture(textures[MATERIAL.metallicRoughnessTexture], inUV).bg;
	}
	float metallic = metallicRoughness.x;
	float roughness = metallicRoughness.y;

	// Normal
	vec3 N;
	if (EXISTS(MATERIAL.normalTexture)) {
		vec3 tangentNormal = texture(textures[MATERIAL.normalTexture], inUV).rgb;
		vec3 scaledNormal = (2.0 * tangentNormal - 1.0) * vec3(MATERIAL.normalScale, MATERIAL.normalScale, 1.0);
		N = normalize(inTBN * scaledNormal);
	}
	else {
		N = vec3(1.0);
	}

	// Occlusion
	float occlusion = 1.0;
	if (EXISTS(MATERIAL.occlusionTexture)) {
		occlusion += (texture(textures[MATERIAL.occlusionTexture], inUV).r - 1.0) * MATERIAL.occlusionStrength;
	}

	// Emissive
	vec3 emissive = MATERIAL.emissiveFactor;
	if (EXISTS(MATERIAL.emissiveTexture)) {
		emissive *= texture(textures[MATERIAL.emissiveTexture], inUV).xyz;
	}

    vec3 V = normalize(ctx.cameraPosition - inPosition);
    vec3 F0 = mix(vec3(0.04), baseColor.rgb, metallic);

    vec3 L = normalize(ctx.light.position - inPosition);
    vec3 H = normalize(V + L);

    float distance = length(ctx.light.position - inPosition);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = ctx.light.color * ctx.light.intensity * attenuation;

	float dotNL = max(dot(N, L), 0.0);
	float dotNV = max(dot(N, V), 0.0);
	float dotNH = max(dot(N, H), 0.0);

    float NDF = DistributionGGX(dotNH, roughness);
    float G = GeometrySmith(dotNL, dotNV, roughness);
    // vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    vec3 F = FresnelSchlickRoughness(max(dot(H, V), 0.0), F0, roughness);

    vec3 specular = NDF * G * F / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 diffuse = (kD * baseColor.rgb / PI);

    vec3 ambient = ctx.light.ambientColor * ctx.light.ambientIntensity * baseColor.rgb * occlusion;
    vec3 color = ambient + (diffuse + specular) * radiance * dotNL + emissive;
	vec3 col = pow(color, vec3(1.0/2.2));
    outColor = vec4(col, 1.0);
}