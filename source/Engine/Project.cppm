#ifdef USE_MODULES
export module Project;
#define _PROJECT_EXPORT export
import Lmath;
import SceneGraph;
import Entity;
import Component;
import stl;
#else
#pragma once
#define _PROJECT_EXPORT
#include "SceneGraph.cppm"
#include "Entity.cppm"
#include "Component.cppm"
#include <string_view>
#endif

_PROJECT_EXPORT
class Project {
public:
	Project() : registry(), sceneGraph(&registry) {}

	Project(const Project&) = delete;
	Project(Project&&) = delete;
	Project& operator=(const Project&) = delete;
	Project& operator=(Project&&) = delete;

	inline Entity CreateEntity(const std::string_view& name = "") {
		auto entity = Entity(&registry, registry.create());
		if (!name.empty()) {
			entity.Add<Component::Name>(name);
		}
		return entity;
	}

	inline SceneGraph& GetSceneGraph() { return sceneGraph; }

	inline void Destroy() {
		registry.clear();
	}

	Registry registry;
private:
	std::string name;
	std::string filepath;
	SceneGraph sceneGraph;
};
