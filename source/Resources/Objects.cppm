#ifdef USE_MODULES
export module Objects;
#define _OBJECTS_EXPORT export
import lmath;
import vulkan_backend;
import std;
#else
#pragma once
#define _OBJECTS_EXPORT
#include "lmath_types.hpp"
#include <vulkan_backend/core.hpp>
#include <cstdint>
#include <vector>
#endif

_OBJECTS_EXPORT
namespace Objects {
using namespace lmath;

struct Vertex {
	vec3  position;
	vec3  normal;
	vec2  uv;
	vec4  color;
	vec4  tangent;
};





} // namespace Objects