#include <SceneGraph.hpp>
#include "Lmath.hpp"

using namespace Lmath;

void SceneGraph::UpdateTransforms(Node &node, const Component::Transform& parentTransform) {
	auto& transform = node.entity.GetComponent<Component::Transform>();
	// printf("\n%s ", node.entity.GetComponent<Component::Name>().name.c_str());
	// printf("Children: %d\n", node.children.size());
	if (transform.dirty ||  parentTransform.dirty ) {
		transform.global = parentTransform.global * transform.local;
	}

	// for (int i = 0; i < 4; i++) {
	// 		for (int j = 0; j < 4; j++) {
	// 			printf("%f ", transform.global[i][j]);
	// 		}
	// 		printf("\n");
	// 	}
	
	for (auto& childIndex : node.children) {
		UpdateTransforms(nodes[childIndex], transform);
	}
	transform.dirty = false;
}


void SceneGraph::DebugPrint() {
	DebugPrintTree(root, 0);
}

void SceneGraph::DebugPrintTree(Node& node, int indent) {
	for (int i = 0; i < indent; i++) {
		printf("  ");
	}
	printf("%s\n", node.entity.GetComponent<Component::Name>().name.c_str());
	for (auto& childIndex : node.children) {
		DebugPrintTree(nodes[childIndex], indent + 1);
	}
}
