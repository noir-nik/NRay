#ifdef USE_MODULES
export module SceneGraph;
#define _SCENEGRAPH_EXPORT export
import Component;
import Entity;
import stl;
import Types;
#else
#pragma once
#define _SCENEGRAPH_EXPORT
#include "Component.cppm"
#include "Entity.cppm"
#endif

_SCENEGRAPH_EXPORT
struct SceneGraph {
	using NodeIndex = uint32_t;
	SceneGraph(const SceneGraph&) = delete;
	SceneGraph& operator=(const SceneGraph&) = delete;
	SceneGraph(SceneGraph&&) = delete;
	SceneGraph& operator=(SceneGraph&&) = delete;

	struct Node {
		Node(Entity entity = {nullptr, Entity::Null}) : entity(entity) {};
		
		Entity entity;
		std::vector<NodeIndex> children;

		friend SceneGraph;

		inline const char* name() const { return entity.Get<Component::Name>().name.c_str(); }
	};

	Node root; // Has scene indices as children
	// std::vector<NodeIndex> scenes; // with top level objects as children
	std::vector<Node> nodes; // only objects

	NodeIndex currentScene = 0;

	Registry* registry;
	SceneGraph(Registry* registry) : registry(registry) {
		root.entity = CreateEntity("Root");
		root.entity.Add<Component::Transform>(false);

		// Add Default scene
		AddScene("Scene");
		root.children.push_back(0);
	}

	inline Entity CreateEntity(const std::string_view& name) {
		Entity entity = {registry, registry->create()};
		entity.Add<Component::Name>(name);
		return entity;
	}

	inline Node& GetCurrentScene() {
		return nodes[currentScene];
	}

	inline Node& Get(NodeIndex index) {	return nodes[index]; }
	inline const Node& Get(NodeIndex index) const { return nodes[index]; }

	NodeIndex AddNode(const std::string_view& name) {
		nodes.emplace_back(CreateEntity(name));
		nodes.back().entity.Add<Component::Transform>(false);
		return nodes.size() - 1;
	}

	void AddScene(const std::string_view& name) {
		nodes.emplace_back(CreateEntity(name));
		nodes.back().entity.Add<Component::Transform>(false);
		// scenes.push_back(nodes.size() - 1);
	}

	inline void resize(std::size_t size) {
		nodes.resize(size, {registry});
	}

	void UpdateTransforms(Node& node, const Component::Transform& parentTransform);
	void DebugPrint();
	void DebugPrintTree(const Node& node, int indent);
};

