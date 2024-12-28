#ifdef USE_MODULES
export module Component;
#define _COMPONENT_EXPORT export
import Lmath.types;
import VulkanBackend;
import Phong;
import Objects;
import stl;
#else
#pragma once
#define _COMPONENT_EXPORT
#include "Lmath.types.cppm"
#include "VulkanBackend.cppm"
#include "Phong.cppm"
#include "Objects.cppm"

#include <cstdint>
#include <string>
#include <vector>
#endif

_COMPONENT_EXPORT
namespace Component {
using namespace Lmath;

struct Name {
    std::string name;
    
    inline Name(const std::string_view& name) : name(name) {}
};

struct Transform {
	mat4 local = {};
	mat4 global = {};

	bool dirty;

	inline Transform(bool dirty = true) : dirty(dirty) {}
};


// struct Mesh {
// 	Mesh(std::shared_ptr<Objects::Mesh> mesh) : mesh(mesh) {}
// 	std::shared_ptr<Objects::Mesh> mesh;
// };
using Mesh = std::shared_ptr<Objects::Mesh>;

// struct Mesh {
// 	inline Mesh(std::string_view name = "") : name(name) {}
// 	std::string name;
// 	std::vector<uint> indices;
// 	std::vector<Objects::Vertex> vertices;
// 	std::vector<Objects::Primitive> primitives;
// 	vkw::Buffer vertexBuffer;
// 	vkw::Buffer indexBuffer;
// };

} // namespace Component
