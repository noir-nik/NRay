#ifndef OPAQUE_H
#define OPAQUE_H

#ifdef ENGINE
#include "Lmath_types.h"
namespace Opaque {
using namespace Lmath;
#endif // ENGINE

#include "Bindless.h"

struct OpaqueLight {
	vec3 position;
	float range;
	vec3 color;
	float intensity;
};

struct OpaqueConstants {
	mat4 viewProj;
	mat4 model;
	OpaqueLight light;

	vec3 cameraPosition;
	int materialBufferRID;
	
	int materialIndex; // in buffer
	int pad[3];
};

#ifdef ENGINE
} // namespace Opaque
#endif

#endif // OPAQUE_H