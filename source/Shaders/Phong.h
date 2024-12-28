#ifndef PHONG_H
#define PHONG_H

#ifdef ENGINE
#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.hpp"
#endif
using namespace Lmath;
#endif

struct PhongLight {
	vec3 position;
	float range;
	vec3 color;
	float intensity;
};

struct PhongMaterial {
    vec3 ambient;
	int pad[1];
    vec3 diffuse;
	int pad1[1];
    vec3 specular;
    float shininess;
};

struct PhongConstants {
	mat4 model;
	mat4 viewProj;
	PhongLight light;
	PhongMaterial material;

	vec3 cameraPosition;
	int pad[1];
};
#endif
