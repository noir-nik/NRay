#ifndef _GPU_TYPES_H
#define _GPU_TYPES_H

#ifdef ENGINE
#include "Lmath_types.h"
namespace GpuTypes {
using namespace Lmath;
#endif // ENGINE

#define TEXTURE_UNDEFINED ~0u

#ifdef ENGINE
#define _EQ_UNDEFINED = TEXTURE_UNDEFINED
#else
#define _EQ_UNDEFINED
#endif

struct Material {
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	float metallicFactor;

	float roughnessFactor;
	uint baseColorTexture _EQ_UNDEFINED;
	uint metallicRoughnessTexture _EQ_UNDEFINED;
	uint normalTexture _EQ_UNDEFINED;

	uint occlusionTexture _EQ_UNDEFINED;
	uint emissiveTexture _EQ_UNDEFINED;
	int pad[2];
};

struct Camera {
	mat4 projectionView;
	vec3 position;
	int pad[1];
};

#ifdef ENGINE
} // namespace GpuTypes
#endif

#undef _EQ_UNDEFINED

#endif // _GPU_TYPES_H