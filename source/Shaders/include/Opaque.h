#ifndef OPAQUE_H
#define OPAQUE_H

#ifdef ENGINE
#ifdef USE_MODULES
import lmath;
#else
#include "lmath_types.hpp"
#endif
namespace Opaque {
using namespace lmath;
#endif // ENGINE


struct OpaqueLight {
	vec3 position;
	float range;
	vec3 color;
	float intensity;
	vec3 ambientColor;
	float ambientIntensity;
};

struct OpaqueConstants {
	mat4 viewProj;
	mat4 model;
	OpaqueLight light;

	vec3 cameraPosition;
	int materialBufferRID;
	
	int materialOffset; // in buffer
	int pad[3];
};

#ifdef ENGINE
} // namespace Opaque
#endif

#endif // OPAQUE_H