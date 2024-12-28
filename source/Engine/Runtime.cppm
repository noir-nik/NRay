#ifdef USE_MODULES
export module Runtime;
#define _RUNTIMECONTEXT_EXPORT export
export import SceneGraph;
import Lmath;
import stl;
#else
#pragma once
#define _RUNTIMECONTEXT_EXPORT
#include "SceneGraph.cppm"
#include "Lmath.cppm"

#include <vector>
#include <variant>
#endif

_RUNTIMECONTEXT_EXPORT
namespace Runtime {
using namespace Lmath;
struct Camera {
	static constexpr float rotation_factor = 0.0025f;
	static constexpr float zoom_factor = 0.01f;
	static constexpr float move_factor = 0.00107f;

	inline Camera(const vec3& position = vec3(3.0f, 3.0f, 10.0f), const vec3& focus = vec3(0.0f, 0.0f, 0.0f), const vec3& up = vec3(0.0f, 1.0f, 0.0f)) : focus(focus) {
		view = lookAt(position, focus, up) | affineInverse;
	}
	
	inline vec3& getRight()    { return view.col(0).xyz(); }
	inline vec3& getUp()       { return view.col(1).xyz(); }
	inline vec3& getForward()  { return view.col(2).xyz(); }
	inline vec3& getPosition() { return view.col(3).xyz(); }

	inline const vec3& getRight()    const { return view.col(0).xyz(); }
	inline const vec3& getUp()       const { return view.col(1).xyz(); }
	inline const vec3& getForward()  const { return view.col(2).xyz(); }
	inline const vec3& getPosition() const { return view.col(3).xyz(); }

	inline void setProj(float fov, int width, int height, float zNear, float zFar) {
		proj = width > height
		? perspectiveX(fov, (float)width / height, zNear, zFar)
		: perspectiveY(fov, (float)height / width, zNear, zFar);

		this->fov = fov;
		this->zNear = zNear;
		this->zFar = zFar;
	}

	inline void updateProj(int width, int height) {
		setProj(fov, width, height, zNear, zFar);
	}

	vec3 focus;
    mat4 view;
	mat4 proj;

	float fov = 60.0f;
	float zNear = 0.01f;
	float zFar = 1000.0f;
};

struct PanelBase {
	Camera camera{};
	ivec4 rect{};
};

struct Viewport : PanelBase {
};

struct Outliner : PanelBase {
	SceneGraph* sceneGraph;
	Outliner(SceneGraph* sceneGraph) : sceneGraph(sceneGraph) {}
};

struct Properties : PanelBase {
	
};

/* 
enum class PanelType {
	Outliner,
	Properties,
	Viewport
};

struct Panel1 {
	PanelType type;

	union {
		Outliner outlinerData;
		Viewport viewportData;
		Properties propertiesData;
	} data;
};
 */
using Panel = std::variant<Viewport, Outliner, Properties>;

struct WindowData {
	bool main = false;
	std::vector<Panel> panels;
	std::span<Panel> tabPanels;
	
	std::vector<Viewport*> viewportsToRender;


	WindowData(bool main) : main(main) {}

	WindowData() = default;
	WindowData(const WindowData&)            = delete;
	WindowData& operator=(const WindowData&) = delete;
	WindowData(WindowData&&)                 = delete;
	WindowData& operator=(WindowData&&)      = delete;
};

struct Data {
	SceneGraph* sceneGraph;
	WindowData* windowData;
};

} // namespace Runtime