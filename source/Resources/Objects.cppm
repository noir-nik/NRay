#ifdef USE_MODULES
export module Objects;
#define _OBJECTS_EXPORT export
import Lmath.types;
import VulkanBackend;
import stl;
#else
#pragma once
#define _OBJECTS_EXPORT
#include "Lmath.types.cppm"
#include "VulkanBackend.cppm"
#include <cstdint>
#include <vector>
#include <memory>
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

struct Primitive {
	uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  vertexOffset;
    uint32_t firstInstance;
	// Material material;
};


struct Mesh {
	inline Mesh(std::string_view name = "") : name(name) {}
	std::string name;
	std::vector<uint> indices;
	std::vector<Vertex> vertices;
	std::vector<Primitive> primitives;
	vkw::Buffer vertexBuffer;
	vkw::Buffer indexBuffer;
};

} // namespace Objects