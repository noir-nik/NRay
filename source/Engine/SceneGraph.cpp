#ifdef USE_MODULES
module SceneGraph;
import Lmath;
#else
#include "Lmath.cppm"
#include "SceneGraph.cppm"
#endif

using namespace Lmath;

void SceneGraph::UpdateTransforms(Node &node, const Component::Transform& parentTransform) {
	auto& transform = node.entity.Get<Component::Transform>();

	if (transform.dirty ||  parentTransform.dirty ) {
		transform.global = parentTransform.global * transform.local;
	}
	
	for (auto& childIndex : node.children) {
		UpdateTransforms(nodes[childIndex], transform);
	}
	transform.dirty = false;
}


void SceneGraph::DebugPrint() {
	DebugPrintTree(root, 0);
}

void SceneGraph::DebugPrintTree(const Node&  node, int indent) {
	for (int i = 0; i < indent; i++) {
		printf("  ");
	}
	printf("%s\n", node.entity.Get<Component::Name>().name.c_str());
	for (auto& childIndex : node.children) {
		DebugPrintTree(nodes[childIndex], indent + 1);
	}
}

