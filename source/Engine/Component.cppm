#ifdef USE_MODULES
module;
#endif

#include "Bindless.h"

#ifdef USE_MODULES
export module Component;
#define _COMPONENT_EXPORT export
import Lmath;
import Objects;
import stl;
import VulkanBackend;
#else
#pragma once
#define _COMPONENT_EXPORT
#include "Lmath_types.h"
#include "Objects.cppm"

#include <cstdint>
#include <string>
#include <vector>
#endif

_COMPONENT_EXPORT
namespace Component {
using namespace Lmath;
using namespace Objects;

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

// using Mesh = std::shared_ptr<Objects::Mesh>;

struct Material {
	enum class AlphaMode : std::uint8_t {
		Opaque,
		Mask,
		Blend,
	};
	std::string name;
	GPUMaterial gpuMaterial;
	uint deviceMaterialID = {};

	bool doubleSided = false;
	float alphaCutoff = 0.5f;
	float emissiveStrength = 1.0f;
	AlphaMode alphaMode = AlphaMode::Opaque;

	// Material(std::string_view name) : name(name) {}
};

struct Primitive {
	uint32_t indexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t firstIndex = 0;
    int32_t  vertexOffset = 0;
    uint32_t firstInstance = 0;
	Material* material = nullptr;
};

struct Mesh {
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	inline Mesh(std::string_view name = "") : name(name) {}
	std::string name;
	std::vector<uint> indices;
	std::vector<Vertex> vertices;
	std::vector<Primitive> primitives;
	vkw::Buffer vertexBuffer;
	vkw::Buffer indexBuffer;
};


} // namespace Component
