module SceneGraph;
import lmath;
import std.compat;
import Log;

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
	LOG_INFO("SceneGraph");
	LOG_INFO("Root: {}", root.entity.Get<Component::Name>().name.c_str());
	LOG_INFO("Children:");
	for (auto& childIndex : root.children) {
		LOG_INFO("{}: {}", childIndex, nodes[childIndex].entity.Get<Component::Name>().name.c_str());
	}
	LOG_INFO("Nodes:");
	for (u32 i = 0; auto& node : nodes) {
		LOG_INFO("===== [{}] {} =====", i, node.entity.Get<Component::Name>().name.c_str());
		if (node.children.size() > 0) {
			LOG_INFO("Children:");
			for (auto& childIndex : node.children) {
				LOG_INFO("{}: {}", childIndex, nodes[childIndex].entity.Get<Component::Name>().name.c_str());
			}
		}
		++i;
	}
	// DebugPrintTree(root, 0);
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

