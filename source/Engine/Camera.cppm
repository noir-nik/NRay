#ifdef USE_MODULES
module;
#endif

#include "GpuTypes.h"

#ifdef USE_MODULES
export module Camera;
#define _CAMERA_EXPORT export
import Lmath;
#else
#pragma once
#define _CAMERA_EXPORT
#include "Lmath.cppm"
#endif

_CAMERA_EXPORT
namespace Engine {
using namespace Lmath;
struct Camera {
	static constexpr float rotation_factor = 0.0025f;
	static constexpr float zoom_factor = 0.01f;
	static constexpr float move_factor = 0.00107f;

    mat4 view;
	mat4 proj;
	GpuTypes::Camera gpuCamera;

	vec3 focus;
	float fov = 60.0f;
	float zNear = 0.01f;
	float zFar = 1000.0f;

	inline Camera(const vec3& position = vec3(3.0f, 3.0f, 10.0f), const vec3& focus = vec3(0.0f, 0.0f, 0.0f), const vec3& up = vec3(0.0f, 1.0f, 0.0f)) : focus(focus) {
		view = lookAt(position, focus, up) | affineInverse;
	}
	
	inline auto getRight()    -> vec3& { return view.col(0).xyz(); }
	inline auto getUp()       -> vec3& { return view.col(1).xyz(); }
	inline auto getForward()  -> vec3& { return view.col(2).xyz(); }
	inline auto getPosition() -> vec3& { return view.col(3).xyz(); }

	inline auto getRight()    const -> vec3 const& { return view.col(0).xyz(); }
	inline auto getUp()       const -> vec3 const& { return view.col(1).xyz(); }
	inline auto getForward()  const -> vec3 const& { return view.col(2).xyz(); }
	inline auto getPosition() const -> vec3 const& { return view.col(3).xyz(); }

	auto getProj()     const -> mat4 const& { return proj; }
	auto getView()     const -> mat4 const& { return view; }
	auto getProjView() const -> mat4 const& { return gpuCamera.projectionView; }
	auto getFocus()    const -> vec3 const& { return focus; }

	inline void setProj(float fov, int width, int height, float zNear, float zFar) {
		proj = width > height
		? perspectiveX(fov, (float)width / height, zNear, zFar)
		: perspectiveY(fov, (float)height / width, zNear, zFar);

		this->fov = fov;
		this->zNear = zNear;
		this->zFar = zFar;

		updateProjView();
	}

	inline void updateProj(int width, int height) {
		setProj(fov, width, height, zNear, zFar);
	}


	void updateProjView() {
		gpuCamera.projectionView = proj * (view | affineInverse);
	}
};

} // namespace Engine