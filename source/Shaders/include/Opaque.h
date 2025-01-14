#ifndef SOURCE_SHADERS_INCLUDE_OPAQUE_H_
#define SOURCE_SHADERS_INCLUDE_OPAQUE_H_

#ifdef ENGINE
import lmath;
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

#endif // SOURCE_SHADERS_INCLUDE_OPAQUE_H_