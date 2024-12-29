#ifndef _GPU_TYPES_H
#define _GPU_TYPES_H

#ifdef ENGINE
#ifdef USE_MODULES
import lmath;
#else
#include "lmath_types.hpp"
#endif
namespace GpuTypes {
using namespace lmath;
#endif // ENGINE

#define TEXTURE_UNDEFINED ~0u

#ifdef ENGINE
#define _EQ_UNDEFINED = TEXTURE_UNDEFINED
#define _EQ_1f    = 1.0f
#define _EQ_05f   = 0.5f
#define _EQ_1vec3 = vec3(1.0f)
#define _EQ_1vec4 = vec4(1.0f)
#else
#define _EQ_UNDEFINED
#define _EQ_1f
#define _EQ_05f
#define _EQ_1vec3
#define _EQ_1vec4
#endif

struct Material {
	vec4 baseColorFactor    _EQ_1vec4;

	vec3 emissiveFactor     _EQ_1vec3;
	float metallicFactor    _EQ_1f;

	float roughnessFactor   _EQ_1f;
	float normalScale       _EQ_1f;
	float occlusionStrength _EQ_1f;
	float alphaCutoff       _EQ_05f;

	uint baseColorTexture         _EQ_UNDEFINED;
	uint metallicRoughnessTexture _EQ_UNDEFINED;
	uint normalTexture            _EQ_UNDEFINED;
	uint occlusionTexture         _EQ_UNDEFINED;

	uint emissiveTexture          _EQ_UNDEFINED;
	int pad[3];
};

struct Camera {
	mat4 projectionViewInv;
	vec3 position;
	int pad[1];
};

#ifdef ENGINE
} // namespace GpuTypes
#endif

#undef _EQ_UNDEFINED
#undef _EQ_1f
#undef _EQ_05f
#undef _EQ_1vec3
#undef _EQ_1vec4

#endif // _GPU_TYPES_H