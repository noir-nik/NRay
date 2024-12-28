#ifndef _GPU_TYPES_H
#define _GPU_TYPES_H

#ifdef ENGINE
#include "Lmath_types.h"
using Lmath::uint;
using Lmath::vec2;
using Lmath::vec3;
using Lmath::vec4;
#endif // ENGINE

struct GPUMaterial {
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	float metallicFactor;

	float roughnessFactor;
	uint baseColorTexture;
	uint metallicRoughnessTexture;
	uint normalTexture;

	uint occlusionTexture;
	uint emissiveTexture;
	int pad[2];
};

#endif