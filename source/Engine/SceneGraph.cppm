#ifdef USE_MODULES
export module SceneGraph;
#define _SCENEGRAPH_EXPORT export
import Component;
import Entity;
import stl;
#else
#pragma once
#define _SCENEGRAPH_EXPORT
#include "Component.cppm"
#include "Entity.cppm"
#endif

_SCENEGRAPH_EXPORT
struct SceneGraph {
	SceneGraph(const SceneGraph&) = delete;
	SceneGraph& operator=(const SceneGraph&) = delete;
	SceneGraph(SceneGraph&&) = delete;
	SceneGraph& operator=(SceneGraph&&) = delete;

	struct Node {
		Node(Entity entity = {nullptr, Entity::Null}) : entity(entity) {};
		
		Entity entity;
		std::vector<uint32_t> children;

		friend SceneGraph;
	};

	Node root; // Has scene indices as children
	std::vector<size_t> scenes; // with top level objects as children
	std::vector<Node> nodes; // only objects

	size_t currentScene = 0;
	Registry* registry;
	SceneGraph(Registry* registry) : registry(registry) {
		root.entity = CreateEntity("Root");
		root.entity.AddComponent<Component::Transform>(false);

		// Add Default scene
		AddScene("Scene");
		root.children.push_back(0);
	}

	inline Entity CreateEntity(const std::string_view& name) {
		Entity entity = {registry, registry->create()};
		entity.AddComponent<Component::Name>(name);
		return entity;
	}

	Node& GetCurrentScene() {
		return nodes[currentScene];
	}

	void AddScene(const std::string_view& name) {
		nodes.emplace_back(CreateEntity(name));
		nodes.back().entity.AddComponent<Component::Transform>(false);
	}

	inline void resize(std::size_t size) {
		nodes.resize(size, {registry});
	}

	void UpdateTransforms(Node& node, const Component::Transform& parentTransform);
	void DebugPrint();
	void DebugPrintTree(const Node& node, int indent);
};

