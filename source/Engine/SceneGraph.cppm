#ifdef USE_MODULES
export module SceneGraph;
#define _SCENEGRAPH_EXPORT export
import Component;
import Entity;
import std;
import Types;
import Structs;
#else
#pragma once
#define _SCENEGRAPH_EXPORT
#include "Component.cppm"
#include "Entity.cppm"
#include "Structs.cppm"
#include "Types.cppm"
#endif

_SCENEGRAPH_EXPORT
struct SceneGraph : Structs::NoCopyNoMove {
	using NodeIndex = u32;

	struct Node {
		Node(Entity entity = {nullptr, Entity::Null}) : entity(entity) {};
		
		Entity entity;
		std::vector<NodeIndex> children;

		friend SceneGraph;

		inline char const* name() const { return entity.Get<Component::Name>().name.c_str(); }
	};

	Node root; // Has scene indices as children
	std::vector<Node> nodes; // only objects

	NodeIndex currentScene = 0;

	Registry* registry;
	SceneGraph(Registry* registry) : registry(registry) {
		root.entity = CreateEntity("Root");
		root.entity.Add<Component::Transform>(false);

		// Add Default scene
		root.children.push_back(AddNode("Scene"));
	}

	inline Entity CreateEntity(const std::string_view& name = "") {
		Entity entity = {registry, registry->create()};
		entity.Add<Component::Name>(name);
		return entity;
	}

	inline Node& GetCurrentScene() {
		return nodes[currentScene];
	}

	inline Node& Get(NodeIndex index) {	return nodes[index]; }
	inline Node const& Get(NodeIndex index) const { return nodes[index]; }

	NodeIndex AddNode(const std::string_view& name) {
		nodes.emplace_back(CreateEntity(name));
		nodes.back().entity.Add<Component::Transform>(false);
		return nodes.size() - 1;
	}

	inline void resize(std::size_t size) {
		nodes.resize(size, {registry});
	}

	void UpdateTransforms(Node& node, const Component::Transform& parentTransform);
	void DebugPrint();
	void DebugPrintTree(Node const& node, int indent);
};

