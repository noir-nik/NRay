#ifdef USE_MODULES
module SceneGraph;
import lmath;
import std.compat;
#else
#include "lmath.hpp"
#include "SceneGraph.cppm"
#endif

using namespace lmath;

void SceneGraph::UpdateTransforms(Node &node, Component::Transform const& parentTransform) {
	auto& transform = node.entity.Get<Component::Transform>();

	if (transform.dirtyGlobal || parentTransform.dirtyGlobal) {
		if (transform.dirtyLocal) {
			transform.update();
		}
		transform.global = parentTransform.global * transform.local;
	}
	
	for (auto& childIndex : node.children) {
		UpdateTransforms(nodes[childIndex], transform);
	}
	transform.dirtyGlobal = false;
}


void SceneGraph::DebugPrint() {
	DebugPrintTree(root, 0);
}

void SceneGraph::DebugPrintTree(Node const& node, int indent) {
	for (int i = 0; i < indent; i++) {
		printf("  ");
	}
	printf("%s\n", node.entity.Get<Component::Name>().name.c_str());
	for (auto& childIndex : node.children) {
		DebugPrintTree(nodes[childIndex], indent + 1);
	}
}

