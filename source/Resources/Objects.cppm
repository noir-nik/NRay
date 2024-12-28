#ifdef USE_MODULES
export module Objects;
#define _OBJECTS_EXPORT export
import Lmath;
import VulkanBackend;
import stl;
#else
#pragma once
#define _OBJECTS_EXPORT
#include "Lmath_types.h"
#include "VulkanBackend.cppm"
#include <cstdint>
#include <vector>
#endif

_OBJECTS_EXPORT
namespace Objects {
using namespace Lmath;

struct Vertex {
	vec3  position;
	vec3  normal;
	vec2  uv;
	vec4  color;
	vec3  tangent;
};





} // namespace Objects