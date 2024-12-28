#ifdef USE_MODULES
module;
#endif

#include "GpuTypes.h"

#ifdef USE_MODULES
export module Component;
#define _COMPONENT_EXPORT export
import lmath;
import Objects;
import std;
import std.compat;
import vulkan_backend;
#else
#pragma once
#define _COMPONENT_EXPORT
#include "lmath.hpp"
#include "Objects.cppm"

#include <cstdint>
#include <string>
#include <vector>
#endif

_COMPONENT_EXPORT
namespace Component {
using namespace lmath;
using namespace Objects;

struct Name {
	std::string name;
	inline Name(const std::string_view& name) : name(name) {}
};

struct Transform {
	enum class Type : std::uint8_t {
		QuaternionXYZW,
		EulerXYZ,
		AxisAngle,
	};

	mat4 local = {};
	mat4 global = {};
	
	vec3 translation = {};
	union {
		quat rotation = {0.0f, 0.0f, 0.0f, 1.0f};
		vec3 rotationEuler;
		vec4 rotationAxisAngle;
	};
	vec3 scale = { 1.0f, 1.0f, 1.0f };

	bool dirtyLocal;
	bool dirtyGlobal = true;

	Type type = Type::QuaternionXYZW;

	inline Transform(bool dirty = true) : dirtyLocal(dirty) {}

	inline auto getTRS() -> std::tuple<vec3&, quat&, vec3&> {
		// setDirty();
		return std::tie(translation, rotation, scale);
	}

	inline auto getTRS() const -> std::tuple<vec3 const&, quat const&, vec3 const&> {
		return std::tie(translation, rotation, scale);
	}

	inline auto getTranslation()       -> vec3& { return translation; }
	inline auto getRotationAxisAngle() -> vec4& { return rotationAxisAngle; }
	inline auto getRotationEuler()     -> vec3& { return rotationEuler; }
	inline auto getRotation()          -> quat& { return rotation; }
	inline auto getScale()             -> vec3& { return scale; }

	inline auto getTranslation()       const -> vec3 const& { return translation; }
	inline auto getRotationAxisAngle() const -> vec4 const& { return rotationAxisAngle; }
	inline auto getRotationEuler()     const -> vec3 const& { return rotationEuler; }
	inline auto getRotation()          const -> quat const& { return rotation; }
	inline auto getScale()             const -> vec3 const& { return scale; }

	inline auto getType() const -> Type { return type; }

	inline auto isDirty()       const -> bool { return dirtyLocal || dirtyGlobal; }
	inline auto isDirtyLocal()  const -> bool { return dirtyLocal;  }
	inline auto isDirtyGlobal() const -> bool { return dirtyGlobal; }

	inline void setType(Type newType) {
		if (dirtyLocal) update();
		switch (newType) {
		case Type::QuaternionXYZW:
			fromMatrix(local);
			break;
		case Type::EulerXYZ:
			fromMatrixEuler(local);
			break;
		case Type::AxisAngle:
			vec3 axis = { 0.0f, 1.0f, 0.0f };
			axis = local * axis;
			// not implemented
			break;
		}

		type = newType;
	}

	inline void setDirty() {
		dirtyLocal = true;
		dirtyGlobal = true;
	}

	inline void fromTRS(vec3 const& t, quat const& r, vec3 const& s) {
		type = Type::QuaternionXYZW;
		translation = t;
		rotation = r;
		scale = s;
		setDirty();
	}

	inline void fromMatrix(mat4 const& m) {
		type = Type::QuaternionXYZW;
		m | std::tie(translation, rotation, scale);
		setDirty();
	}

	inline void fromMatrixEuler(mat4 const& m) {
		type = Type::EulerXYZ;
		m | std::tie(translation, rotationEuler, scale);
		setDirty();
	}

	inline void update() {
		mat4 m = scale4x4(scale);
		switch (type) {
		case Type::QuaternionXYZW:
			m = m | rotate(rotation);
			break;
		case Type::EulerXYZ:
			m = m | rotateZ(rotationEuler.z) | rotateY(rotationEuler.y) | rotateX(rotationEuler.x);
			break;
		case Type::AxisAngle:
			m = m | rotate(rotationAxisAngle.xyz(), rotationAxisAngle.w);
			break;
		}

		m = m | translate(translation);
		local = m;
		dirtyLocal = false;
	}
};

// using Mesh = std::shared_ptr<Objects::Mesh>;

struct Material {
	enum class AlphaMode : std::uint8_t {
		Opaque,
		Mask,
		Blend,
	};
	std::string name;
	GpuTypes::Material gpuMaterial;
	uint deviceMaterialID = {};

	bool doubleSided = false;
	float alphaCutoff = 0.5f;
	float emissiveStrength = 1.0f;
	AlphaMode alphaMode = AlphaMode::Opaque;

	// Material(std::string_view name) : name(name) {}
	void update() {
		
	}
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
	vb::Buffer vertexBuffer;
	vb::Buffer indexBuffer;
};


} // namespace Component
