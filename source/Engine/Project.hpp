#pragma once
#ifdef USE_MODULES
import Lmath;
import SceneGraph;
import Entity;
#else
#include "Lmath.cppm"
#include "SceneGraph.cppm"
#include "Entity.cppm"
#endif

class Project {
public:
	Project() : registry(), sceneGraph(&registry) {}

	Project(const Project&) = delete;
	Project(Project&&) = delete;
	Project& operator=(const Project&) = delete;
	Project& operator=(Project&&) = delete;

	inline Entity CreateEntity(const std::string_view& name) {
		auto entity = Entity(&registry, registry.create());
		// entity.AddComponent<Component::Name>(name);
		return entity;
	}

	SceneGraph& GetSceneGraph() { return sceneGraph; }

	void Destroy() {
		registry.clear();
	}

	entt::registry registry;
private:
	std::string name;
	std::string filepath;
	SceneGraph sceneGraph;
};
