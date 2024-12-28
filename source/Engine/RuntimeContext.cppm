#ifdef USE_MODULES
export module RuntimeContext;
#define _RUNTIMECONTEXT_EXPORT export
export import SceneGraph;
export import Objects;
#else
#pragma once
#define _RUNTIMECONTEXT_EXPORT
#include "SceneGraph.cppm"
#include "Objects.cppm"
#endif

_RUNTIMECONTEXT_EXPORT
struct RuntimeContext {
	SceneGraph* sceneGraph;
	Objects::Camera* camera;

	RuntimeContext() = default;
	RuntimeContext(const RuntimeContext&) = delete;
	RuntimeContext(RuntimeContext&&) = delete;
	RuntimeContext& operator=(const RuntimeContext&) = delete;
	RuntimeContext& operator=(RuntimeContext&&) = delete;
};

