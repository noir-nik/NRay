#ifndef OPAQUE_H
#define OPAQUE_H

#ifdef ENGINE
#include "Lmath_types.h"
namespace Opaque {
using namespace Lmath;
#endif // ENGINE

struct OpaqueLight {
	vec3 position;
	float range;
	vec3 color;
	float intensity;
};

struct OpaqueMaterial {
	struct {
		vec4 baseColorFactor = vec4(1.0f);
		Texture baseColorTexture;
		float metallicFactor = float(1.0f);
		float roughnessFactor = float(1.0f);
		Texture metallicRoughnessTexture;
		// extensions
		// extras
	} pbrMetallicRoughness;
	
	Texture normalTexture;
	Texture occlusionTexture;

	Texture emissiveTexture;
	vec3 emissiveFactor = vec3(0.0f);

	std::string alphaMode = "OPAQUE";
	float alphaCutoff = 0.5f;

	bool doubleSided = false;
};


struct Model {
	mat4 model;
};

struct OpaqueConstants {
	mat4 viewProj;
	int modelID;
	OpaqueLight light;
	OpaqueMaterial material;

	vec3 cameraPosition;
	int pad[1];
};

#ifdef ENGINE
} // namespace Opaque
#endif

#endif // OPAQUE_H