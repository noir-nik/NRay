#ifndef SOURCE_SHADERS_INCLUDE_PHONG_H_
#define SOURCE_SHADERS_INCLUDE_PHONG_H_

#ifdef ENGINE
import lmath;
namespace Phong {
using lmath::vec3;
using lmath::vec4;
using lmath::mat4;
#endif // ENGINE

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

#ifdef ENGINE
} // namespace Phong
#endif

#endif // SOURCE_SHADERS_INCLUDE_PHONG_H_
