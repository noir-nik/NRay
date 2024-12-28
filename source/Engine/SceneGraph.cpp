#include <SceneGraph.hpp>
#include "Lmath.hpp"

using namespace Lmath;

void SceneGraph::UpdateTransforms(Node &node, const Component::Transform& parentTransform) {
	auto& transform = node.entity.GetComponent<Component::Transform>();
	if (transform.dirty ||  parentTransform.dirty ) {
		transform.global = parentTransform.global * transform.local;
	}

	for (auto& childIndex : node.children) {
		UpdateTransforms(nodes[childIndex], transform);
	}
	transform.dirty = false;
}
