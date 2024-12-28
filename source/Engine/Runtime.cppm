#ifdef USE_MODULES
export module Runtime;
#define _RUNTIMECONTEXT_EXPORT export
export import SceneGraph;
export import Objects;
import Types;
import Lmath;
#else
#pragma once
#define _RUNTIMECONTEXT_EXPORT
#include "SceneGraph.cppm"
#include "Objects.cppm"
#include "Types.cppm"
#include "Lmath.cppm"
#endif

_RUNTIMECONTEXT_EXPORT
namespace Runtime {
using namespace Lmath;
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

struct Viewport {
	int width;
	int height;
};

struct Context {
	SceneGraph* sceneGraph;
	Camera* camera;

	Context() = default;
	Context(const Context&) = delete;
	Context(Context&&) = delete;
	Context& operator=(const Context&) = delete;
	Context& operator=(Context&&) = delete;
};

} // namespace Runtime