#ifdef USE_MODULES
export module Project;
#define _PROJECT_EXPORT export
import Lmath.types;
import SceneGraph;
import Entity;
import stl;
#else
#pragma once
#define _PROJECT_EXPORT
#include "SceneGraph.cppm"
#include "Entity.cppm"

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

	inline Entity CreateEntity(const std::string_view& name) {
		auto entity = Entity(&registry, registry.create());
		// entity.Add<Component::Name>(name);
		return entity;
	}

	SceneGraph& GetSceneGraph() { return sceneGraph; }

	void Destroy() {
		registry.clear();
	}

	Registry registry;
private:
	std::string name;
	std::string filepath;
	SceneGraph sceneGraph;
};
