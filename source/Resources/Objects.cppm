#ifdef USE_MODULES
export module Objects;
#define _OBJECTS_EXPORT export
import Lmath;
import VulkanBackend;
import stl;
#else
#pragma once
#define _OBJECTS_EXPORT
#include "Lmath.cppm"
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

struct Camera {
	static constexpr float rotation_factor = 0.0025f;
	static constexpr float zoom_factor = 0.01f;
	static constexpr float move_factor = 0.00107f;

	inline Camera(const vec3& position, const vec3& focus = vec3(0.0f, 0.0f, 0.0f), const vec3& up = vec3(0.0f, 1.0f, 0.0f)) : focus(focus) {
		// view = affineInverse4x4(lookAt(position, focus, up));
		view = lookAt(position, focus, up).affineInverse();
	}
	
	inline vec3& getRight()    { return view.col(0).xyz(); }
	inline vec3& getUp()       { return view.col(1).xyz(); }
	inline vec3& getForward()  { return view.col(2).xyz(); }
	inline vec3& getPosition() { return view.col(3).xyz(); }

	inline const vec3& getRight()    const { return view.col(0).xyz(); }
	inline const vec3& getUp()       const { return view.col(1).xyz(); }
	inline const vec3& getForward()  const { return view.col(2).xyz(); }
	inline const vec3& getPosition() const { return view.col(3).xyz(); }
	
	vec3 focus;
    mat4 view;
	mat4 proj;
};
}