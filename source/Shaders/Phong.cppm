#ifndef PHONG_H
#define PHONG_H

#ifdef ENGINE
#ifdef USE_MODULES
export module Phong;
#define _PHONG_EXPORT export
import Lmath;
#else
#include "Lmath_types.h"
#define _PHONG_EXPORT
#endif // USE_MODULES
namespace Phong {
using namespace Lmath;
#else
#define _PHONG_EXPORT
#endif // ENGINE

_PHONG_EXPORT
struct PhongLight {
	vec3 position;
	float range;
	vec3 color;
	float intensity;
};

_PHONG_EXPORT
struct PhongMaterial {
    vec3 ambient;
	int pad[1];
    vec3 diffuse;
	int pad1[1];
    vec3 specular;
    float shininess;
};

_PHONG_EXPORT
struct PhongConstants {
	mat4 model;
	mat4 viewProj;
	PhongLight light;
	PhongMaterial material;

	vec3 cameraPosition;
	int pad[1];
};

#ifdef ENGINE
} // namespace Phong
#endif

#endif // PHONG_H
