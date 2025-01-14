export module Project;
import lmath;
import SceneGraph;
import Entity;
import Component;
import Structs;
import std;


export
class Project : Structs::NoCopyNoMove {
public:
	Project() : registry(), sceneGraph(&registry) {}

	inline Entity CreateEntity(const std::string_view& name = "") {
		auto entity = Entity(&registry, registry.create());
		if (!name.empty()) {
			entity.Add<Component::Name>(name);
		}
		return entity;
	}

	[[nodiscard]] inline auto GetSceneGraph() -> SceneGraph& { return sceneGraph; }

	[[nodiscard]] inline auto GetRegistry() -> Registry& { return registry; }

	inline void Destroy() {
		registry.clear();
	}

private:
	std::string name;
	std::string filepath;
	Registry registry;
	SceneGraph sceneGraph;
};
