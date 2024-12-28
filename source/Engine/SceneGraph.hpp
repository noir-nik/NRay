#pragma once

#include "Component.hpp"
#include "Entity.hpp"

struct SceneGraph {
	SceneGraph(const SceneGraph&) = delete;
	SceneGraph& operator=(const SceneGraph&) = delete;
	SceneGraph(SceneGraph&&) = delete;
	SceneGraph& operator=(SceneGraph&&) = delete;

	struct Node {
		Node(Entity entity = {nullptr, Entity::Null}) : entity(entity) {};
		
		Entity entity;
		std::vector<uint32_t> children;

		void UpdateTransform(const Lmath::mat4& parentTransform);
		friend SceneGraph;
	};
	Node scenes; // Has scenes as children
	std::vector<Node> nodes; // with indexes
	Registry* registry;
	SceneGraph(Registry* registry) : registry(registry) {}

	inline Entity CreateEntity(const std::string_view& name) {
		Entity entity = {registry, registry->create()};
		entity.AddComponent<Component::Name>(name);
		return entity;
	}

	inline void resize(std::size_t size) {
		nodes.resize(size, {registry});
	}

	void UpdateTransforms(Node &node, const Component::Transform& parentTransform);
};

