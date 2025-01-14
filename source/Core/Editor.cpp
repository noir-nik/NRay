module Editor;
import std;
import std.compat;
import lmath;
import Types;
import vulkan_backend;
import imgui;
import Log;
import Camera;
import Entity;
import Runtime;
import Structs;
import Component;
import entt;
import imgui_impl_glfw;
import imgui_impl_vulkan;

#define STYLE_PATH "assets/"

namespace Editor {
using namespace lmath;
// MSVC doesn't compile without these
// when using imgui module
// NOLINTBEGIN(misc-unused-using-decls)
using ::ImGuiWindowFlags_;
using ::ImGuiChildFlags_;
using ::ImGuiItemFlags_;
using ::ImGuiInputTextFlags_;
using ::ImGuiTreeNodeFlags_;
using ::ImGuiPopupFlags_;
using ::ImGuiSelectableFlags_;
using ::ImGuiComboFlags_;
using ::ImGuiTabBarFlags_;
using ::ImGuiTabItemFlags_;
using ::ImGuiFocusedFlags_;
using ::ImGuiHoveredFlags_;
using ::ImGuiDockNodeFlags_;
using ::ImGuiDragDropFlags_;
using ::ImGuiDataType_;
using ::ImGuiDir;
using ::ImGuiSortDirection;
using ::ImGuiKey;
using ::ImGuiInputFlags_;
using ::ImGuiConfigFlags_;
using ::ImGuiBackendFlags_;
using ::ImGuiCol_;
using ::ImGuiStyleVar_;
using ::ImGuiButtonFlags_;
using ::ImGuiColorEditFlags_;
using ::ImGuiSliderFlags_;
using ::ImGuiMouseButton_;
using ::ImGuiMouseCursor_;
using ::ImGuiMouseSource;
using ::ImGuiCond_;
using ::ImGuiTableFlags_;
using ::ImGuiTableColumnFlags_;
using ::ImGuiTableRowFlags_;
using ::ImGuiTableBgTarget_;
using ::ImGuiTableSortSpecs;
using ::ImGuiTableColumnSortSpecs;
using ::ImNewWrapper;
using ::ImGuiStyle;
using ::ImGuiKeyData;
using ::ImGuiIO;
using ::ImGuiInputTextCallbackData;
using ::ImGuiSizeCallbackData;
using ::ImGuiWindowClass;
using ::ImGuiPayload;
using ::ImGuiOnceUponAFrame;
using ::ImGuiTextFilter;
using ::ImGuiTextBuffer;
using ::ImGuiStoragePair;
using ::ImGuiStorage;
using ::ImGuiListClipper;
using ::ImColor;
using ::ImGuiMultiSelectFlags_;
using ::ImGuiMultiSelectIO;
using ::ImGuiSelectionRequestType;
using ::ImGuiSelectionRequest;
using ::ImGuiSelectionBasicStorage;
using ::ImGuiSelectionExternalStorage;
using ::ImDrawCmd;
using ::ImDrawVert;
using ::ImDrawCmdHeader;
using ::ImDrawChannel;
using ::ImDrawListSplitter;
using ::ImDrawFlags_;
using ::ImDrawListFlags_;
using ::ImDrawList;
using ::ImDrawData;
using ::ImFontConfig;
using ::ImFontGlyph;
using ::ImFontGlyphRangesBuilder;
using ::ImFontAtlasCustomRect;
using ::ImFontAtlasFlags_;
using ::ImFontAtlas;
using ::ImFont;
using ::ImGuiViewportFlags_;
using ::ImGuiViewport;
using ::ImGuiPlatformIO;
using ::ImGuiPlatformMonitor;
using ::ImGuiPlatformImeData;
// NOLINTEND(misc-unused-using-decls)
struct Context;

void drawWindowRects(ImGuiWindow* window);

struct Tab {
	std::string name;
	std::vector<Panel> panels;
	void Draw(Data const& ctx);
};

static constexpr u32 NodeNotFound = ~0u;

struct Context : Structs::NoCopyNoMove {
	using NodeIndex = SceneGraph::NodeIndex;
	using Transform = Component::Transform;
	using Material = Component::Material;
	using Mesh = Component::Mesh;
	
	bool showStyleEditor = false;
	bool showDemoWindow = false;
	bool showDebugWindow = false;
	bool showStyleSelector = false;
	bool showFontSelector = false;
	ImGuiStyle defaultStyle;
	ImGuiStyle style;

	SceneGraph* sceneGraph;

	std::vector<NodeIndex> selectedNodes;
	NodeIndex activeNode = NodeNotFound;
	std::vector<Tab> tabs;

	void SetupWindow(WindowData& windowData);

	void MainMenu(Data const& ctx);
	void RenderUI();
	void StyleWindow(); 
	void StyleEditor();

	void Draw(Data const& ctx);

	void DebugWindow(Engine::Camera& camera);
	auto ViewportPanel(Viewport& ctx) -> bool;
	auto OutlinerPanel(Outliner& ctx) -> bool;
	auto PropertiesPanel(Properties& ctx) -> bool;
	
	auto SaveStyle(char const* filename, ImGuiStyle const& style) -> bool;
	auto LoadStyle(char const* filename, ImGuiStyle& style) -> bool;

	auto FindSelected(NodeIndex nodeIndex) -> u32;
	void Select(NodeIndex nodeIndex);
	void DisplayNode(NodeIndex const nodeIndex, ImGuiTreeNodeFlags flags);
	auto DisplayTransform(Transform& transform) -> bool;
	auto DisplayMaterial(Material& material) -> bool;
	auto DisplayMesh(Mesh& mesh) -> bool;

} editorContext;
inline static auto GetStyleVarName(ImGuiStyleVar idx) -> char const*;

struct PanelVisitor {
	bool operator()(Viewport& ctx) const {
		return editorContext.ViewportPanel(ctx);
	}
	
	bool operator()(Outliner& ctx) const {
		return editorContext.OutlinerPanel(ctx);
	}
	
	bool operator()(Properties& ctx) const {
		return editorContext.PropertiesPanel(ctx);
	}
};
/* 
void Tab::Draw(Data const& ctx) {
	for (auto& panel : panels) {
		(editorContext.*panel.draw)(ctx);
	}
}
 */

void Context::SetupWindow(WindowData& windowData) {
	if (windowData.main) {
		// windowData.panels.emplace_back(Outliner{});
		// windowData.panels.emplace_back(Viewport{.viewport = {0, 0, 500, 500}});
	}
}

void Context::MainMenu(Data const& ctx) {
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.09f, 0.09f, 0.09f, 1.0f));
	if (!ImGui::BeginMainMenuBar()) {
		LOG_WARN("Failed to create main menu bar");
		ImGui::PopStyleColor();
		return;
	}
	ImGui::PopStyleColor();

	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureMouse |= io.WantCaptureKeyboard;
	
	if (ImGui::BeginMenu("File")) {
		if (ImGui::BeginMenu("New")) {
			ImGui::MenuItem("New Text File");
			ImGui::MenuItem("New Project", "Ctrl+N");
			ImGui::MenuItem("New Window", "Ctrl+Shift+N");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Open")) {
			ImGui::MenuItem("File", "Ctrl+O");
			ImGui::MenuItem("Folder", "Ctrl+Shift+O");
			ImGui::EndMenu();
		}
		ImGui::MenuItem("Recent");
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) {
		ImGui::MenuItem("Undo");
		ImGui::MenuItem("Redo");
		ImGui::MenuItem("Style Editor", "", &showStyleEditor);
		ImGui::MenuItem("Debug Window", "", &showDebugWindow);
		ImGui::MenuItem("Demo Window", "", &showDemoWindow);
		// ImGui::MenuItem("Style Selector", "", &ShowStyleSelector);
		// ImGui::MenuItem("Font Selector", "", &ShowFontSelector);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help")) {
		ImGui::MenuItem("Documentation");
		ImGui::MenuItem("About");
		ImGui::EndMenu();
	}

	/* if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_None)){
		if (ImGui::BeginTabItem("View3D")) {
			if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoDecoration)) {
				ImGui::Text("Viewport size: (%.1f, %.1f)", ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
				ImGui::Text("Viewport is hovered: %d", ImGui::IsWindowHovered());
			}
			ImGui::End();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Mesh")) {
			// stuff in tab here too
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animation")) {
			// stuff in tab here
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Lighting")) {
			// stuff in tab here
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Node Editor")) {
			// stuff in tab here
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	} */

	PanelVisitor visitor;
	static Editor::Tab* lastTab = nullptr;
	bool tabUpdated = false;
	if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_None)){
		for (auto& tab : tabs) {
			if (ImGui::BeginTabItem(tab.name.c_str())) {
				if (lastTab != &tab) {
					tabUpdated = true;
					lastTab = &tab;
				}
				for (auto& panel : tab.panels) {
					bool updated = std::visit(visitor, panel);
					if (std::holds_alternative<Viewport>(panel) && (updated || tabUpdated)) {
						ctx.windowData->viewportsToRender.push_back(&std::get<Viewport>(panel));
					}
					else if (std::holds_alternative<Outliner>(panel) && (updated || tabUpdated)) {
						for (auto& v : tab.panels) {
							if (std::holds_alternative<Viewport>(v)) {
								ctx.windowData->viewportsToRender.push_back(&std::get<Viewport>(v));
							}
						}
					}
				}
				ImGui::EndTabItem();
				ctx.windowData->tabPanels = tab.panels;
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::EndMainMenuBar();
}

void Context::RenderUI() {
	ImGui::Begin("Scene Panel", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	ImGui::Text("Scene Objects:");
	ImGui::Separator();

	static char const* sceneObjects[] = { "Camera", "Light", "Cube", "Sphere", "Plane" };
	for (size_t i = 0; i < std::size(sceneObjects); i++) {
		ImGui::Selectable(sceneObjects[i]);
	}
	ImGui::End();

	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	bool isHovered = ImGui::IsWindowHovered();

	ImGui::Text("Viewport size: (%.1f, %.1f)", viewportSize.x, viewportSize.y);
	ImGui::Text("Viewport is %s", isHovered ? "hovered" : "not hovered");
	
	ImGui::End();

	ImGui::Begin("Slider", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	static float sliderValue = 0.5f;
	bool changed = ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 1.0f);
	if (changed) ImGui::Text("Slider changed!");
	ImGui::Text("Slider value: %f", sliderValue);
	ImGui::End();
}

void Context::StyleWindow() {
	if (ImGui::Begin("Style Window", nullptr, ImGuiWindowFlags_NoCollapse)) {
		if (ImGui::Button("Save Style")) {
			if (SaveStyle(STYLE_PATH"style.ini", ImGui::GetStyle())) {
				style = ImGui::GetStyle();
			} else {
				LOG_WARN("Failed to save style");
			};
		}
		if (ImGui::Button("Load Style")) {
			if (LoadStyle(STYLE_PATH"style.ini", ImGui::GetStyle())) {
				LOG_INFO("Loaded style");
				style = ImGui::GetStyle();
			} else {
				LOG_WARN("Failed to load style");
				ImGui::GetStyle() = style;
			}
		}
		if (ImGui::Button("Style Editor")) {
			showStyleEditor = true;
		}
	}

	ImGui::End();
}

void showMat4(float4x4 const& m) {
	for (int i = 0; i < 4; i++) {
		ImGui::Text("%.2f, %.2f, %.2f, %.2f",
			m[i, 0], m[i, 1], m[i, 2], m[i, 3]);
	}
};

void Context::DebugWindow(Engine::Camera& camera) {

	if (ImGui::Begin("Debug Window", 0, ImGuiWindowFlags_NoDocking)) {
		ImGui::Text("Camera Focus: (%.2f, %.2f, %.2f)", camera.focus.x, camera.focus.y, camera.focus.z);

		// static float position[3] = { 0.0f, 2.0f, -30.0f };
		// static float rotation[3] = { 0.0f, 0.0f, 0.0f };
		// static float cameraUp[3] = { 0.0f, 1.0f, 0.0f };

		
		float3 position, rotation, scale;
		camera.view | affineInverse | std::tie(position, rotation, scale);

		// ImGui::SliderFloat3("Camera Position", position.M, -50.0f, 50.0f);
		// ImGui::SliderFloat3("Camera Focus", rotation.M, -50.0f, 50.0f);
		// ImGui::SliderFloat3("Camera Up", cameraUp, -1.0f, 1.0f);

		// camera.view = inverse4x4(lookAt(vec3(position[0], position[1], position[2]),
		// 	vec3(rotation[0], rotation[1], rotation[2]),
		// 	vec3(cameraUp[0], cameraUp[1], cameraUp[2])));

		// camera.view = lookAt(vec3(position[0], position[1], position[2]),
		// 	vec3(rotation[0], rotation[1], rotation[2]),
		// 	vec3(cameraUp[0], cameraUp[1], cameraUp[2]));


		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
		ImGui::Text("Camera Rotation: (%.2f, %.2f, %.2f)", rotation.x, rotation.y, rotation.z);
		ImGui::Text("Camera Scale: (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);

		ImGui::Text("View");
		showMat4(camera.view);
		/*
		ImGui::Separator();
		ImGui::Text("Projection");
		showMat4(camera.proj); 
		*/
	}

	ImGui::End();
}

auto Context::FindSelected(NodeIndex nodeIndex) -> u32 {
	auto it = std::find_if(selectedNodes.begin(), selectedNodes.end(), [&](NodeIndex const other) {
		return nodeIndex == other;
	});
	return it == selectedNodes.end() ? NodeNotFound : it - selectedNodes.begin();
}

void Context::Select(NodeIndex nodeIndex) {
	bool holdingCtrl = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
	auto index = FindSelected(nodeIndex);
	if (index != NodeNotFound && holdingCtrl) {
		selectedNodes.erase(selectedNodes.begin() + index);
	}
	if (!holdingCtrl) {
		selectedNodes.clear();
	}
	selectedNodes.push_back(nodeIndex);
	activeNode = nodeIndex;
}

ImVec4 header_color;
ImVec4 selected_hovered;
ImVec4 active_col;
ImVec4 active_hovered;

void Context::DisplayNode(NodeIndex const nodeIndex, ImGuiTreeNodeFlags flags) {
	
	auto const& node = sceneGraph->Get(nodeIndex);
	ImGuiTreeNodeFlags child_flags = flags;
	if (node.children.size() == 0) {
		child_flags |= ImGuiTreeNodeFlags_Leaf;
	}
	bool is_active = nodeIndex == activeNode;
	bool is_selected = FindSelected(nodeIndex) != NodeNotFound;
	// bool is_hovered = ImGui::IsItemHovered();

	ImGui::PushID(nodeIndex);
	if (is_active) {
		child_flags |= ImGuiTreeNodeFlags_Bullet;
	}
	if (is_selected) {
		child_flags |= ImGuiTreeNodeFlags_Selected;
	}
	bool open = ImGui::TreeNodeEx(node.name(), child_flags);
	// if (ImGui::IsItemDeactivated()) {
	// if (ImGui::IsItemClicked()) {
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		Select(nodeIndex);
	}
	if (open) {
		for (auto const& child : node.children) {
			DisplayNode(child, flags); // Recursively display children
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}


auto Context::DisplayTransform(Transform& transform) -> bool {
	using Type = Transform::Type;
	auto& position = transform.getTranslation();
	auto& scale = transform.getScale();
	bool updated = false;

	auto constexpr dragSpeed = 0.05f;

	float3 rotationEulerDegrees;
	float4 rotationAxisDegrees;
	
	ImGui::SeparatorText("Transform");
	updated |= ImGui::DragFloat3("Position", position.M, dragSpeed);

	switch (transform.getType()) {
	case Type::QuaternionXYZW:
		updated |= ImGui::DragFloat4("Rotation", transform.getRotation().M, dragSpeed);
		break;
	case Type::EulerXYZ:
		rotationEulerDegrees = transform.getRotationEuler() / DEG_TO_RAD;
		updated |= ImGui::DragFloat3("Rotation", rotationEulerDegrees.M, dragSpeed / DEG_TO_RAD);
		transform.getRotationEuler() = rotationEulerDegrees * DEG_TO_RAD;
		break;
	case Type::AxisAngle:
		rotationAxisDegrees = transform.getRotationAxisAngle() / DEG_TO_RAD;
		updated |= ImGui::DragFloat4("Rotation", rotationAxisDegrees.M, dragSpeed / DEG_TO_RAD);
		transform.getRotationAxisAngle() = rotationAxisDegrees * DEG_TO_RAD;
		break;
	}
	updated |= ImGui::DragFloat3("Scale", scale.M, dragSpeed);

	int newType = static_cast<int>(transform.type);
	bool updated_type = ImGui::Combo("##", &newType, "Quaternion\0Euler\0AxisAngle\0\0");
	updated |= updated_type;
	// if (updated) {ImGui::Text("Updated"); LOG_INFO("Updated"); }
	if (updated_type) transform.setType(static_cast<Type>(newType));
/* 
	ImGui::Text("Decompose");
	auto [t,r,s] = transform.local | decompose;
	auto [_,re,_s] = transform.local | decomposeEuler;
	re /= DEG_TO_RAD;
	ImGui::Text("Translation: (%.2f, %.2f, %.2f)", t.x, t.y, t.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f %.2f)", r.x, r.y, r.z, r.w);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", re.x, re.y, re.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", s.x, s.y, s.z);
 */
	if (updated) {
		transform.setDirty();
		transform.update();
	}
	// showMat4(transform.local);

	// ImGui::Text("Children: %d", node.children.size());
	return updated;
}

auto Context::DisplayMaterial(Material& material) -> bool {
	bool updated = false;
	auto& gpuMaterial = material.gpuMaterial;

	// ImGui::Text("Material %s", material.name.c_str());
	// ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	// if (ImGui::BeginChild("Material3")) {

		updated |= ImGui::ColorEdit4("Color", gpuMaterial.baseColorFactor.M);
		updated |= ImGui::ColorEdit3("Emissive", gpuMaterial.emissiveFactor.M);
		updated |= ImGui::DragFloat("Metallic", &gpuMaterial.metallicFactor, 0.01f, 0.0f, 1.0f);
		updated |= ImGui::DragFloat("Roughness", &gpuMaterial.roughnessFactor, 0.01f, 0.0f, 1.0f);
		updated |= ImGui::DragFloat("Normal Scale", &gpuMaterial.normalScale, 0.01f, 0.0f, 1.0f);
		updated |= ImGui::DragFloat("Occlusion Strength", &gpuMaterial.occlusionStrength, 0.01f, 0.0f, 1.0f);
		updated |= ImGui::DragFloat("Alpha Cutoff", &gpuMaterial.alphaCutoff, 0.01f, 0.0f, 1.0f);
	// }
	// ImGui::EndChild();
	
	// ImGui::PopStyleColor();
	// if (updated) mesh.material.setDirty();
	// if (updated) mesh.material.update();
	return updated;
}

auto Context::DisplayMesh(Mesh& mesh) -> bool {
	// ImGui::Text("Material %s", material.name.c_str());
	bool updated = false;
	// ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	static int selected = -1;

	if (ImGui::BeginChild("Material", {0.0f, 80.0f}, ImGuiChildFlags_ResizeY | ImGuiChildFlags_FrameStyle)) {

		for (int i = 0; auto& p : mesh.primitives)
		{
			if (!p.material) continue;
			// ImGui::ColorButton("##", ImColor(dummy_col), ImGuiColorEditFlags_NoTooltip, color_button_sz);
 			// ImGui::SameLine();
			ImGui::PushID(i);
			ImVec2 color_button_sz(ImGui::GetFontSize(), ImGui::GetFontSize());
			ImGui::ColorButton("##", (ImColor)(*p.material->gpuMaterial.baseColorFactor.M), ImGuiColorEditFlags_NoTooltip, color_button_sz);
			ImGui::SameLine();
			if (ImGui::Selectable(p.material->name.c_str(), selected == i)){
				// ImGui::Text("%s", p.material->name.c_str());
				// ImGui::Text("%s", p.material->name.c_str());
				selected = i;
			}
			ImGui::PopID();
			++i;
		}
	}
	ImGui::EndChild();
	// ImGui::SameLine();
	char const* u = "AAA";

	if (selected != -1) {
		if (ImGui::BeginCombo("##material", mesh.primitives[selected].material->name.c_str(), ImGuiComboFlags_HeightRegular)) {
		auto view = sceneGraph->registry->view<Material>();
		
		for (int i = 0; auto m : view) {
			auto& material = sceneGraph->registry->get<Material>(m);
			if (ImGui::Selectable(material.name.c_str(), selected == i)){
				// ImGui::Text("%s", p.material->name.c_str());
				// ImGui::Text("%s", p.material->name.c_str());
				selected = i;
			}
			++i;
		}
		ImGui::EndCombo();
	}
		updated |= DisplayMaterial(*mesh.primitives[selected].material);
	}



	

	

	

	return updated;
}

auto ProcessViewportInput(Viewport& ctx, ImGuiKeyChord mods, float rotation_sign) -> bool {
	auto& io = ImGui::GetIO();
	if (io.MouseDown[ImGuiMouseButton_Left] + io.MouseDown[ImGuiMouseButton_Right] + io.MouseDown[ImGuiMouseButton_Middle] > 1) {
		return 0;
	}
	auto& camera = ctx.camera;
	auto& camera_pos = camera.getPosition();
	auto const& camera_right = camera.getRight();
	auto const& camera_up = camera.getUp();
	auto const& camera_forward = camera.getForward();

	vec2 deltaPos = {-io.MouseDelta.x, -io.MouseDelta.y};

	if (io.MouseDown[ImGuiMouseButton_Right]) {
		auto res = mods & ImGuiMod_Alt;
		if(mods & ImGuiMod_Alt) {
			auto zoom_factor = camera.zoom_factor * length(camera_pos - camera.focus);
			auto movement = (zoom_factor * deltaPos.x) * camera_forward;
			camera.view = camera.view | translate(movement);
			// camera.focus += movement;
		} else {
			camera_pos -= camera.focus;
			// camera.view = rotate4x4(camera_up, deltaPos.x * camera.rotation_factor)
			//	 * rotate4x4(camera_right, deltaPos.y * camera.rotation_factor)
			//	 * camera.view; // trackball
			camera.view = camera.view
				| rotate(camera_right, deltaPos.y * camera.rotation_factor)
				| rotateY(rotation_sign * deltaPos.x * camera.rotation_factor);
			camera_pos += camera.focus;
		}
		// window->AddFramesToDraw(1);
		return 1;
	} 
	if (io.MouseDown[ImGuiMouseButton_Middle]) {
		auto move_factor = camera.move_factor * length(camera_pos - camera.focus);
		auto movement = move_factor * (camera_up * -deltaPos.y + camera_right * deltaPos.x); 
		// printf("%f %f %f\n", movement.x, movement.y, movement.z);
		// camera.focus += movement;
		camera.view = camera.view | translate(movement);
		// window->AddFramesToDraw(1);
		return 1;
	}

	if (io.MouseDown[ImGuiMouseButton_Left]) {
		// window->AddFramesToDraw(1);		
		return 1;
	}
	return 0;
}
// static inline bool operator!=(ImVec2 const& lhs, ImVec2 const& rhs)  { return lhs.x != rhs.x || lhs.y != rhs.y; }

auto Context::ViewportPanel(Viewport& ctx) -> bool {
	// ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
	// ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGuiWindowFlags flags {
		ImGuiWindowFlags_NoCollapse
		// | ImGuiWindowFlags_NoMove
		// | ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_MenuBar
		| ImGuiWindowFlags_NoBackground
	};
	bool viewport_changed = false;
	if (ImGui::Begin("Viewport", nullptr, flags) ) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::Button("Add")) {
				//
			}
			ImGui::EndMenuBar();
		}
		auto w = ImGui::GetCurrentWindowRead();
		auto r = w->ClipRect;
		auto new_rect = ivec4 (r.Min.x, r.Min.y, r.Max.x, r.Max.y);
		viewport_changed = ctx.rect != new_rect;
		if (viewport_changed) {
			// LOG_INFO("Viewport changed");
			ctx.rect = new_rect;
			// ctx.rectChanged = true;
			ctx.camera.updateProj(ctx.rect.z - ctx.rect.x, ctx.rect.w - ctx.rect.y);
		}
		auto& io = ImGui::GetIO();
		bool hoveringRect = ImGui::IsMouseHoveringRect(r.Min, r.Max);
		bool contentHoverable = ImGui::IsWindowContentHoverable(w);
		bool windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
		bool dragging_l = ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f);
		bool dragging_r = ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f);
		bool dragging_m = ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f);
		// ImGui::ResetMouseDragDelta()
		// ImGui::ResetMouseDragDelta()
		// auto& p = io.MouseClickedPos[1];
		auto dragging = /* dragging_l ||  */dragging_r || dragging_m;
		static int prev_dragging = 0;
		static bool proc = 0;
		static float rotation_sign;
		static ImGuiKeyChord mods;
		if (dragging != prev_dragging) {
			if (dragging && hoveringRect && contentHoverable && windowHovered) {
				proc = 1;
				rotation_sign = ctx.camera.getUp().y > 0 ? 1.0f : -1.0f;
				mods = io.KeyMods;
			} else {
				proc = 0;
			}
			prev_dragging = !prev_dragging;
		}

		// drawWindowRects(w);

		if (proc) {
			viewport_changed |= ProcessViewportInput(ctx, mods, rotation_sign);
		}
		// ImGui::Text("Viewport changed: %s", viewport_changed ? "true" : "false");
	}
	// ImGui::PopStyleVar();
	// ImGui::PopStyleColor();
	ImGui::End();
	if (showDebugWindow) DebugWindow(ctx.camera);	

	return viewport_changed;
}


auto Context::OutlinerPanel(Outliner& ctx) -> bool {
	ImGuiTreeNodeFlags const parent_flags {
		ImGuiTreeNodeFlags_OpenOnArrow       |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanFullWidth |
		ImGuiTreeNodeFlags_FramePadding
	};
	bool updated = false;
	/* 
	ImGui::Separator();
	if (activeNode != -1) {
		ImGui::Text("Active: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", sceneGraph->Get(activeNode).name());
		ImGui::Separator();
	}
	ImGui::Text("Selected: ");
	for (auto const& node : selectedNodes) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", sceneGraph->Get(node).name());
	}
*/
	ImGuiWindowFlags const outliner_flags {
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoTitleBar
	};
	if (ImGui::Begin("Outliner", nullptr, outliner_flags) ) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Copy")) {
					// Copy();
				}
				if (ImGui::MenuItem("Paste")) {
					// Paste();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		if (ImGui::TreeNodeEx("Scene Graph", parent_flags | ImGuiTreeNodeFlags_DefaultOpen)) {
			for (auto const& nodeIndex : sceneGraph->GetCurrentScene().children) {
				DisplayNode(nodeIndex, parent_flags);
			}
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	ImGui::End();
	return updated;
}

auto Context::PropertiesPanel(Properties& ctx) -> bool {
	ImGuiWindowFlags const properties_flags {
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoTitleBar
	};

	bool updated = false;
	
	if (ImGui::Begin("Properties", nullptr, properties_flags) ) {

		if (activeNode != NodeNotFound) {
			auto& node = sceneGraph->Get(activeNode);
			auto& transform = node.entity.Get<Component::Transform>();
			updated |= DisplayTransform(transform);

			if (node.entity.Has<Component::Mesh*>()) {
				auto& mesh = node.entity.Get<Component::Mesh*>();
				ImGui::SeparatorText("Materials");
				updated |= DisplayMesh(*mesh);
			}
		}
	}
	ImGui::End();
	return updated;
}

/* 
namespace ImGui{
void TextWithHoverColor(ImVec4 col, char const* fmt, ...)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    // Format text
    va_list args;
    va_start(args, fmt);
    char const* text_begin = g.TempBuffer.Data;
    char const* text_end = g.TempBuffer.Data + ImFormatStringV(g.TempBuffer.Data, std::size(g.TempBuffer), fmt, args);
    va_end(args);

    // Layout
    ImVec2 const text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    ImVec2 const text_size = CalcTextSize(text_begin, text_end);
    ImRect bb(text_pos.x, text_pos.y, text_pos.x + text_size.x, text_pos.y + text_size.y);
    ItemSize(text_size, 0.0f);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    bool hovered = IsItemHovered();
    if (hovered)
        PushStyleColor(ImGuiCol_Text, col);
    RenderText(bb.Min, text_begin, text_end, false);
    if (hovered)
        PopStyleColor();
}
}

void Hover_Example() {
	if (ImGui::Begin("Hover Example")) {
		char const* item_label = "Hover me!";
		ImGui::TextWithHoverColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "%s", item_label);
	}
	ImGui::End();
}
 */

void Context::Draw(Data const& ctx) {
	// ctx.windowData->viewportsToRender.clear();
	if (ctx.windowData->viewportsToRender.size() > 1 )
		ctx.windowData->viewportsToRender.resize(1);
	if (ctx.windowData->main) {
		editorContext.MainMenu(ctx);
	}
	// sizeof (Panel1);
	// sizeof (Panel);
	
	if (showDemoWindow) ImGui::ShowDemoWindow();
	if (showStyleEditor) StyleEditor(); 
	// ctx.RenderUI();
}

void Draw(Data const& ctx) {
	editorContext.Draw(ctx);
}

void ImGuiNewFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void ImGuiShutdown() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void BeginFrame() {
	ImGuiNewFrame();
	ImGui::NewFrame();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0));
	ImGui::DockSpaceOverViewport(ImGui::GetWindowDockID(), 0, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar);
	ImGui::PopStyleColor();
}

UiDrawData* EndFrame() {
	ImGui::Render();
	return static_cast<UiDrawData*>(ImGui::GetDrawData());
}

void Setup(SceneGraph* sceneGraph){

	editorContext.defaultStyle = ImGui::GetStyle();

	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiStyle& ref = editorContext.style;

	if (editorContext.LoadStyle(STYLE_PATH"style.ini", style)) {
		LOG_INFO("Loaded style");
		ref = style;
	} else {
		LOG_WARN("Failed to load style");
		style = editorContext.defaultStyle;
		ref = editorContext.defaultStyle;
	}

	header_color = ImGui::GetStyleColorVec4(ImGuiCol_Header);
	// selected_hovered = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);
	// active_col = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);
	// active_hovered = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);

	selected_hovered = { 1.0f, 0.0f, 0.0f, 1.0f };
	active_col = { 0.0f, 1.0f, 0.0f, 1.0f };
	active_hovered = { 0.0f, 0.0f, 1.0f, 1.0f };


	editorContext.sceneGraph = sceneGraph;

	editorContext.tabs = {
		{"View3d", {
			{Outliner{}},
			{Properties{}},
			{Viewport{}},
		}},
		{"Mesh", {
			{Outliner{}},
			{Viewport{}},
		}},
	};
}

void SetupWindow(WindowData& windowData){
	editorContext.SetupWindow(windowData);
}

UiStyle* GetStyle() {
	return static_cast<UiStyle*>(&editorContext.style);
}

static inline auto GetStyleVarName(ImGuiStyleVar idx) -> char const* {
	switch (idx) {
	case ImGuiStyleVar_Alpha:                       return "Alpha";                       // float
	case ImGuiStyleVar_DisabledAlpha:               return "DisabledAlpha";               // float
	case ImGuiStyleVar_WindowPadding:               return "WindowPadding";               // ImVec2
	case ImGuiStyleVar_WindowRounding:              return "WindowRounding";              // float
	case ImGuiStyleVar_WindowBorderSize:            return "WindowBorderSize";            // float
	case ImGuiStyleVar_WindowMinSize:               return "WindowMinSize";               // ImVec2
	case ImGuiStyleVar_WindowTitleAlign:            return "WindowTitleAlign";            // ImVec2
	case ImGuiStyleVar_ChildRounding:               return "ChildRounding";               // float
	case ImGuiStyleVar_ChildBorderSize:             return "ChildBorderSize";             // float
	case ImGuiStyleVar_PopupRounding:               return "PopupRounding";               // float
	case ImGuiStyleVar_PopupBorderSize:             return "PopupBorderSize";             // float
	case ImGuiStyleVar_FramePadding:                return "FramePadding";                // ImVec2
	case ImGuiStyleVar_FrameRounding:               return "FrameRounding";               // float
	case ImGuiStyleVar_FrameBorderSize:             return "FrameBorderSize";             // float
	case ImGuiStyleVar_ItemSpacing:                 return "ItemSpacing";                 // ImVec2
	case ImGuiStyleVar_ItemInnerSpacing:            return "ItemInnerSpacing";            // ImVec2
	case ImGuiStyleVar_IndentSpacing:               return "IndentSpacing";               // float
	case ImGuiStyleVar_CellPadding:                 return "CellPadding";                 // ImVec2
	case ImGuiStyleVar_ScrollbarSize:               return "ScrollbarSize";               // float
	case ImGuiStyleVar_ScrollbarRounding:           return "ScrollbarRounding";           // float
	case ImGuiStyleVar_GrabMinSize:                 return "GrabMinSize";                 // float
	case ImGuiStyleVar_GrabRounding:                return "GrabRounding";                // float
	case ImGuiStyleVar_TabRounding:                 return "TabRounding";                 // float
	case ImGuiStyleVar_TabBorderSize:               return "TabBorderSize";               // float
	case ImGuiStyleVar_TabBarBorderSize:            return "TabBarBorderSize";            // float
	case ImGuiStyleVar_TabBarOverlineSize:          return "TabBarOverlineSize";          // float
	case ImGuiStyleVar_TableAngledHeadersAngle:     return "TableAngledHeadersAngle";     // float
	case ImGuiStyleVar_TableAngledHeadersTextAlign: return "TableAngledHeadersTextAlign"; // ImVec2
	case ImGuiStyleVar_ButtonTextAlign:             return "ButtonTextAlign";             // ImVec2
	case ImGuiStyleVar_SelectableTextAlign:         return "SelectableTextAlign";         // ImVec2
	case ImGuiStyleVar_SeparatorTextBorderSize:     return "SeparatorTextBorderSize";     // float
	case ImGuiStyleVar_SeparatorTextAlign:          return "SeparatorTextAlign";          // ImVec2
	case ImGuiStyleVar_SeparatorTextPadding:        return "SeparatorTextPadding";        // ImVec2
	case ImGuiStyleVar_DockingSeparatorSize:        return "DockingSeparatorSize";        // float
	case ImGuiStyleVar_COUNT:                       return "COUNT";
	default:
		DEBUG_ASSERT(0, "Unknown ImGuiStyleVar");
		return "";
	}
}

// void ExtraStyle () {
//     WindowMenuButtonPosition= ImGuiDir_Left; 
//     TouchExtraPadding       = ImVec2(0,0);   
//     ColumnsMinSpacing       = 6.0f;          
//     LogSliderDeadzone       = 4.0f;          
//     TabMinWidthForCloseButton = 0.0f;        
//     ColorButtonPosition     = ImGuiDir_Right;
//     DisplayWindowPadding    = ImVec2(19,19); 
//     DisplaySafeAreaPadding  = ImVec2(3,3);   
//     DockingSeparatorSize    = 2.0f;          
//     MouseCursorScale        = 1.0f;          
//     AntiAliasedLines        = true;          
//     AntiAliasedLinesUseTex  = true;          
//     AntiAliasedFill         = true;          
//     CurveTessellationTol    = 1.25f;         
//     CircleTessellationMaxError = 0.30f;      

//     // Behaviors
//     HoverStationaryDelay    = 0.15f;
//     HoverDelayShort         = 0.15f;
//     HoverDelayNormal        = 0.40f;
//     HoverFlagsForTooltipMouse = ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled;
//     HoverFlagsForTooltipNav = ImGuiHoveredFlags_NoSharedDelay | ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled;
// }


auto Context::SaveStyle(char const* filename, ImGuiStyle const& style) -> bool {
	// Write .style file
	FILE* f = fopen(filename, "wt");
	if (!f)  return false;

	for (auto i = 0; i != ImGuiStyleVar_COUNT; i++) {
		const ImGuiDataVarInfo* var_info = ImGui::GetStyleVarInfo(i);
		if (var_info->Type != ImGuiDataType_Float) {DEBUG_ASSERT(0, "Invalid type"); continue;}
		float* pvar = (float*)var_info->GetVarPtr((void*)&style);
		float* pvarDefault = (float*)var_info->GetVarPtr((void*)&defaultStyle);
		if (var_info->Count == 1) {
		} else {
		if (*pvar != *pvarDefault) fprintf(f, "[%s]\n%1.3f\n", GetStyleVarName(i), *pvar);
			if (pvar[0] != pvarDefault[0] || pvar[1] != pvarDefault[1]) {
			}
			fprintf(f, "[%s]\n%1.3f %1.3f\n", GetStyleVarName(i), pvar[0], pvar[1]);
		}
	}

	if (style.WindowMenuButtonPosition != defaultStyle.WindowMenuButtonPosition)      fprintf(f, "[WindowMenuButtonPosition]\n   %1.3d\n",       style.WindowMenuButtonPosition);
    if (style.TouchExtraPadding[0] != defaultStyle.TouchExtraPadding[0] ||
		style.TouchExtraPadding[1] != defaultStyle.TouchExtraPadding[1])              fprintf(f, "[TouchExtraPadding]\n          %1.3f %1.3f\n", style.TouchExtraPadding[0], style.TouchExtraPadding[1]);
    if (style.ColumnsMinSpacing != defaultStyle.ColumnsMinSpacing)                    fprintf(f, "[ColumnsMinSpacing]\n          %1.3f\n",       style.ColumnsMinSpacing);
    if (style.LogSliderDeadzone != defaultStyle.LogSliderDeadzone)                    fprintf(f, "[LogSliderDeadzone]\n          %1.3f\n",       style.LogSliderDeadzone);
    if (style.TabMinWidthForCloseButton != defaultStyle.TabMinWidthForCloseButton)    fprintf(f, "[TabMinWidthForCloseButton]\n  %1.3f\n",       style.TabMinWidthForCloseButton);
    if (style.ColorButtonPosition != defaultStyle.ColorButtonPosition)                fprintf(f, "[ColorButtonPosition]\n        %d\n",          style.ColorButtonPosition);
    if (style.DisplayWindowPadding[0] != defaultStyle.DisplayWindowPadding[0] ||
		style.DisplayWindowPadding[1] != defaultStyle.DisplayWindowPadding[1])        fprintf(f, "[DisplayWindowPadding]\n       %1.3f %1.3f\n", style.DisplayWindowPadding[0], style.DisplayWindowPadding[1]);
    if (style.DisplaySafeAreaPadding[0] != defaultStyle.DisplaySafeAreaPadding[0] ||
		style.DisplaySafeAreaPadding[1] != defaultStyle.DisplaySafeAreaPadding[1])    fprintf(f, "[DisplaySafeAreaPadding]\n     %1.3f %1.3f\n", style.DisplaySafeAreaPadding[0], style.DisplaySafeAreaPadding[1]);
    if (style.MouseCursorScale != defaultStyle.MouseCursorScale)                      fprintf(f, "[MouseCursorScale]\n           %1.3f\n",       style.MouseCursorScale);
    if (style.AntiAliasedLines != defaultStyle.AntiAliasedLines)                      fprintf(f, "[AntiAliasedLines]\n           %d\n",          style.AntiAliasedLines);
    if (style.AntiAliasedLinesUseTex != defaultStyle.AntiAliasedLinesUseTex)          fprintf(f, "[AntiAliasedLinesUseTex]\n     %d\n",          style.AntiAliasedLinesUseTex);
    if (style.AntiAliasedFill != defaultStyle.AntiAliasedFill)                        fprintf(f, "[AntiAliasedFill]\n            %d\n",          style.AntiAliasedFill);
    if (style.CurveTessellationTol != defaultStyle.CurveTessellationTol)              fprintf(f, "[CurveTessellationTol]\n       %1.3f\n",       style.CurveTessellationTol);
    if (style.CircleTessellationMaxError != defaultStyle.CircleTessellationMaxError)  fprintf(f, "[CircleTessellationMaxError]\n %1.3f\n",       style.CircleTessellationMaxError);
    if (style.HoverStationaryDelay != defaultStyle.HoverStationaryDelay)              fprintf(f, "[HoverStationaryDelay]\n       %1.3f\n",       style.HoverStationaryDelay);
    if (style.HoverDelayShort != defaultStyle.HoverDelayShort)                        fprintf(f, "[HoverDelayShort]\n            %1.3f\n",       style.HoverDelayShort);
    if (style.HoverDelayNormal != defaultStyle.HoverDelayNormal)                      fprintf(f, "[HoverDelayNormal]\n           %1.3f\n",       style.HoverDelayNormal);
    if (style.HoverFlagsForTooltipMouse != defaultStyle.HoverFlagsForTooltipMouse)    fprintf(f, "[HoverFlagsForTooltipMouse]\n  %d\n",          style.HoverFlagsForTooltipMouse);
    if (style.HoverFlagsForTooltipNav != defaultStyle.HoverFlagsForTooltipNav)        fprintf(f, "[HoverFlagsForTooltipNav]\n    %d\n",          style.HoverFlagsForTooltipNav);
		
	for (size_t i = 0; i != ImGuiCol_COUNT; i++){
		ImVec4 const& c = style.Colors[i];
		if (c.x != defaultStyle.Colors[i].x || c.y != defaultStyle.Colors[i].y || c.z != defaultStyle.Colors[i].z || c.w != defaultStyle.Colors[i].w) {
			fprintf(f, "[%s]\n%1.3f %1.3f %1.3f %1.3f\n", ImGui::GetStyleColorName(i), c.x,c.y,c.z,c.w);
		}
	}

	fprintf(f,"\n");
	fclose(f);
	
	return true;
}

auto Context::LoadStyle(char const* filename, ImGuiStyle& style) -> bool {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
	
	style = defaultStyle;

    std::string line;
	std::string name = "";
	name.reserve(64);
	void* pvar = nullptr;
	// int* pInt = nullptr;
	// float* pFloat = nullptr;
	ImGuiDataVarInfo var_info{ImGuiDataType_COUNT};
	
	// if (!std::getline(file, line)) return false;
	int line_number = 0;
	while (std::getline(file, line)) {
		++line_number;
		// printf("line: %s\n", line.data());
		if (var_info.Type == ImGuiDataType_COUNT) {
			bool found = 0;
			size_t name_start = line.find_first_of("[");
			if (name_start == std::string::npos) continue;
			size_t name_end = line.find_first_of("]", name_start + 1);
			if (name_end == std::string::npos) continue;
			name = std::string_view(line.begin() + name_start + 1, line.begin() + name_end);
			if (name.empty()) continue;
			// printf("name: %s\n", name.data());

			for (auto i = 0; i != ImGuiStyleVar_COUNT; i++) {
					var_info = *ImGui::GetStyleVarInfo(i);
			if (name == GetStyleVarName(i)) {
					pvar = var_info.GetVarPtr((void*)&style);
					found = 1;
					break;
				}
			}
			if (found) continue;
			if (name == "WindowMenuButtonPosition")   { var_info = {ImGuiDataType_Float, 1}; pvar = &style.WindowMenuButtonPosition;   found = true; }
			if (name == "TouchExtraPadding")          { var_info = {ImGuiDataType_Float, 2}; pvar = &style.TouchExtraPadding;          found = true; }
			if (name == "ColumnsMinSpacing")          { var_info = {ImGuiDataType_Float, 1}; pvar = &style.ColumnsMinSpacing;          found = true; }
			if (name == "LogSliderDeadzone")          { var_info = {ImGuiDataType_Float, 1}; pvar = &style.LogSliderDeadzone;          found = true; }
			if (name == "TabMinWidthForCloseButton")  { var_info = {ImGuiDataType_Float, 1}; pvar = &style.TabMinWidthForCloseButton;  found = true; }
			if (name == "ColorButtonPosition")        { var_info = {ImGuiDataType_S32,   1}; pvar = &style.ColorButtonPosition;        found = true; }
			if (name == "DisplayWindowPadding")       { var_info = {ImGuiDataType_Float, 2}; pvar = &style.DisplayWindowPadding;       found = true; }
			if (name == "DisplaySafeAreaPadding")     { var_info = {ImGuiDataType_Float, 2}; pvar = &style.DisplaySafeAreaPadding;     found = true; }
			if (name == "MouseCursorScale")           { var_info = {ImGuiDataType_Float, 1}; pvar = &style.MouseCursorScale;           found = true; }
			if (name == "AntiAliasedLines")           { var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedLines;           found = true; }
			if (name == "AntiAliasedLinesUseTex")     { var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedLinesUseTex;     found = true; }
			if (name == "AntiAliasedFill")            { var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedFill;            found = true; }
			if (name == "CurveTessellationTol")       { var_info = {ImGuiDataType_Float, 1}; pvar = &style.CurveTessellationTol;       found = true; }
			if (name == "CircleTessellationMaxError") { var_info = {ImGuiDataType_Float, 1}; pvar = &style.CircleTessellationMaxError; found = true; }
			if (name == "HoverStationaryDelay")       { var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverStationaryDelay;       found = true; }
			if (name == "HoverDelayShort")            { var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverDelayShort;            found = true; }
			if (name == "HoverDelayNormal")           { var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverDelayNormal;           found = true; }
			if (name == "HoverFlagsForTooltipMouse")  { var_info = {ImGuiDataType_S32,   1}; pvar = &style.HoverFlagsForTooltipMouse;  found = true; }
			if (name == "HoverFlagsForTooltipNav")    { var_info = {ImGuiDataType_S32,   1}; pvar = &style.HoverFlagsForTooltipNav;    found = true; }
			if (found) continue;
			for (int i = 0; i < ImGuiCol_COUNT; i++) {
				if (name == ImGui::GetStyleColorName(i)) {
					var_info.Type = ImGuiDataType_Float;
					var_info.Count = 4;
					pvar = &style.Colors[i];
					found = 1;
					break;
				}
			}
			if (!found) {
				LOG_WARN("Skipped unknown parameter [{}] in \"{}\" at line {}", name, filename, line_number);
			}
		} else {
			bool success = false;
			int tmp;
			switch (var_info.Type) {
				case ImGuiDataType_Bool:{
					if (sscanf(line.c_str(), "%d", &tmp) == 1) {
						auto p = (bool*)pvar;
						*p = tmp;
						continue;
					}
				} break;
				case ImGuiDataType_S32: {
					if (sscanf(line.c_str(), "%d", &tmp) == 1) {
						auto p = (int*)pvar;
						*p = tmp;
						continue;
					}
				} break;
				case ImGuiDataType_Float: {
					float x, y, z, w;
					auto p = (float*)pvar;
					ImU32 num_scanned = sscanf(line.c_str(), "%f %f %f %f", &x, &y, &z, &w);
					if (num_scanned == var_info.Count) {
						switch (var_info.Count) {
							case 4: p[3] = w; [[fallthrough]];
							case 3: p[2] = z; [[fallthrough]];
							case 2: p[1] = y; [[fallthrough]];
							case 1: p[0] = x; 
							var_info.Type = ImGuiDataType_COUNT;
							continue;
							default: break;
						}
					}
				} break;
			}
			var_info.Type = ImGuiDataType_COUNT;
			LOG_WARN("Invalid value for parameter [{}]: \"{}\", expected {} value(s), skipping this parameter", name, line, var_info.Count);
		}
    }

	if (var_info.Type != ImGuiDataType_COUNT) {
		LOG_WARN("No value found for parameter [{}]: expected {} value(s), skipping this parameter", name, var_info.Count);
	}
    file.close();
    return true;
}

void drawWindowRects(ImGuiWindow* window) {
	auto drawList          = ImGui::GetForegroundDrawList();
	auto OuterRectClipped  = window->OuterRectClipped;
	auto InnerRect         = window->InnerRect;
	auto InnerClipRect     = window->InnerClipRect;
	auto WorkRect          = window->WorkRect;
	auto ParentWorkRect    = window->ParentWorkRect;
	auto ClipRect          = window->ClipRect;
	auto ContentRegionRect = window->ContentRegionRect;
	drawList->AddRect(OuterRectClipped.Min, OuterRectClipped.Max, ImGui::Col32(255, 0, 0, 255));
	drawList->AddRect(InnerRect.Min, InnerRect.Max, ImGui::Col32(0, 255, 0, 255));
	drawList->AddRect(InnerClipRect.Min, InnerClipRect.Max, ImGui::Col32(0, 0, 255, 255));
	drawList->AddRect(WorkRect.Min, WorkRect.Max, ImGui::Col32(255, 255, 0, 255));
	drawList->AddRect(ParentWorkRect.Min, ParentWorkRect.Max, ImGui::Col32(0, 255, 255, 255));
	drawList->AddRect(ClipRect.Min, ClipRect.Max, ImGui::Col32(255, 0, 255, 255));
	drawList->AddRect(ContentRegionRect.Min, ContentRegionRect.Max, ImGui::Col32(0, 0, 0, 255));
}

void HelpMarker(char const* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void Context::StyleEditor() {
	if(ImGui::Begin("Style Editor", &showStyleEditor)) {
		// You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
		// (without a reference style pointer, we will use one compared locally as a reference)
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiStyle& ref = this->style; 
		// static ImGuiStyle ref_saved_style;

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

		// if (ImGui::ShowStyleSelector("Colors##Selector"))
			// ref_saved_style = style;
		ImGui::ShowFontSelector("Fonts##Selector");

		// Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
		if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
			style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
		{ bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
		ImGui::SameLine();
		{ bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder",  &border)) { style.FrameBorderSize  = border ? 1.0f : 0.0f; } }
		ImGui::SameLine();
		{ bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder",  &border)) { style.PopupBorderSize  = border ? 1.0f : 0.0f; } }

		if (ImGui::Button("Save Style")) {
			if (SaveStyle(STYLE_PATH"style.ini", style)) {
				ref = style;
			} else {
				LOG_WARN("Failed to save style");
			};
		}
		if (ImGui::Button("Load Style")) {
			if (LoadStyle(STYLE_PATH"style.ini", style)) {
				LOG_INFO("Loaded style");
				ref = style;
			} else {
				LOG_WARN("Failed to load style");
				// style = ref;
			}
		}
		if (ImGui::Button("Reset Style")) {
			ImGui::GetStyle() = defaultStyle; // Reset to default style
		}
		ImGui::SameLine();
		HelpMarker(
			"Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
			"Use \"Export\" below to save them somewhere.");

		ImGui::Separator();

		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Sizes"))
			{
				ImGui::SeparatorText("Main");
				ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
				ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");

				ImGui::SeparatorText("Borders");
				ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("TabBarBorderSize", &style.TabBarBorderSize, 0.0f, 2.0f, "%.0f");
				ImGui::SliderFloat("TabBarOverlineSize", &style.TabBarOverlineSize, 0.0f, 2.0f, "%.0f");
				ImGui::SameLine(); HelpMarker("Overline is only drawn over the selected tab when ImGuiTabBarFlags_DrawSelectedOverline is set.");

				ImGui::SeparatorText("Rounding");
				ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");

				ImGui::SeparatorText("Tables");
				ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderAngle("TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, +50.0f);
				ImGui::SliderFloat2("TableAngledHeadersTextAlign", (float*)&style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");

				ImGui::SeparatorText("Widgets");
				ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
				int window_menu_button_position = style.WindowMenuButtonPosition + 1;
				if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
					style.WindowMenuButtonPosition = (ImGuiDir)(window_menu_button_position - 1);
				ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
				ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
				ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
				ImGui::SliderFloat("SeparatorTextBorderSize", &style.SeparatorTextBorderSize, 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat2("SeparatorTextAlign", (float*)&style.SeparatorTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::SliderFloat2("SeparatorTextPadding", (float*)&style.SeparatorTextPadding, 0.0f, 40.0f, "%.0f");
				ImGui::SliderFloat("DockingSeparatorSize", &style.DockingSeparatorSize, 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");

				ImGui::SeparatorText("Tooltips");
				for (int n = 0; n < 2; n++)
					if (ImGui::TreeNodeEx(n == 0 ? "HoverFlagsForTooltipMouse" : "HoverFlagsForTooltipNav"))
					{
						ImGuiHoveredFlags* p = (n == 0) ? &style.HoverFlagsForTooltipMouse : &style.HoverFlagsForTooltipNav;
						ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNone", p, ImGuiHoveredFlags_DelayNone);
						ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayShort", p, ImGuiHoveredFlags_DelayShort);
						ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNormal", p, ImGuiHoveredFlags_DelayNormal);
						ImGui::CheckboxFlags("ImGuiHoveredFlags_Stationary", p, ImGuiHoveredFlags_Stationary);
						ImGui::CheckboxFlags("ImGuiHoveredFlags_NoSharedDelay", p, ImGuiHoveredFlags_NoSharedDelay);
						ImGui::TreePop();
					}

				ImGui::SeparatorText("Misc");
				ImGui::SliderFloat2("DisplayWindowPadding", (float*)&style.DisplayWindowPadding, 0.0f, 30.0f, "%.0f"); ImGui::SameLine(); HelpMarker("Apply to regular windows: amount which we enforce to keep visible when moving near edges of your screen.");
				ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f"); ImGui::SameLine(); HelpMarker("Apply to every windows, menus, popups, tooltips: amount where we avoid displaying contents. Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Colors"))
			{
				static int output_dest = 0;
				static bool output_only_modified = true;
				if (ImGui::Button("Export"))
				{
					if (output_dest == 0)
						ImGui::LogToClipboard();
					else
						ImGui::LogToTTY();
					ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;\n");
					for (int i = 0; i < ImGuiCol_COUNT; i++) {
						ImVec4 const& col = style.Colors[i];
						char const* name = ImGui::GetStyleColorName(i);
						if (!output_only_modified || memcmp(&col, &ref.Colors[i], sizeof(ImVec4)) != 0)
							ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);\n",
								name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
					}
					ImGui::LogFinish();
				}
				ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
				ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

				static ImGuiTextFilter filter;
				filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

				static ImGuiColorEditFlags alpha_flags = 0;
				if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
				if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
				if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
				HelpMarker(
					"In the color list:\n"
					"Left-click on color square to open color picker,\n"
					"Right-click to open edit options menu.");

				ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
				ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
				ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					char const* name = ImGui::GetStyleColorName(i);
					if (!filter.PassFilter(name))
						continue;
					ImGui::PushID(i);
	#ifndef IMGUI_DISABLE_DEBUG_TOOLS
					if (ImGui::Button("?"))
						ImGui::DebugFlashStyleColor((ImGuiCol)i);
					ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
					ImGui::SameLine();
	#endif
					ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
					if (memcmp(&style.Colors[i], &defaultStyle.Colors[i], sizeof(ImVec4)) != 0)
					{
						// Tips: in a real user application, you may want to merge and use an icon font into the main font,
						// so instead of "Save"/"Revert" you'd use icons!
						// Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
						// ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
						ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = defaultStyle.Colors[i]; }
					}
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
					ImGui::TextUnformatted(name);
					ImGui::PopID();
				}
				int iii = 13490135;
				auto color_select = [&](char const* name, ImVec4& color) -> void {
					ImGui::PushID(iii++);
					if (ImGui::Button("?")){

					}
					ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
					ImGui::SameLine();
					ImGui::ColorEdit4("##color", (float*)&color, ImGuiColorEditFlags_AlphaBar | alpha_flags);
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
					ImGui::TextUnformatted(name);
					ImGui::PopID();
				};
				// auto& header_color = ImGui::GetStyleColorVec4(ImGuiCol_Header);
				// auto& selected_hovered = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);
				// auto& active_col = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);
				// auto& active_hovered = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);

				color_select("Selected Hovered", selected_hovered);
				color_select("Active Hovered", active_hovered);
				color_select("Active", active_col);
				
				ImGui::PopItemWidth();
				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Fonts"))
			{
				ImGuiIO& io = ImGui::GetIO();
				ImFontAtlas* atlas = io.Fonts;
				HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
				// ImGui::ShowFontAtlas(atlas);

				// Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
				// (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
				float const MIN_SCALE = 0.3f;
				float const MAX_SCALE = 2.0f;
				HelpMarker(
					"Those are old settings provided for convenience.\n"
					"However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
					"rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
					"Using those settings here will give you poor quality results.");
				static float window_scale = 1.0f;
				ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
				if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) // Scale only this window
					ImGui::SetWindowFontScale(window_scale);
				ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
				ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Rendering"))
			{
				ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
				ImGui::SameLine();
				HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

				ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
				ImGui::SameLine();
				HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

				ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
				ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
				ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
				if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

				// When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
				ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				bool const show_samples = ImGui::IsItemActive();
				if (show_samples)
					ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
				if (show_samples && ImGui::BeginTooltip())
				{
					ImGui::TextUnformatted("(R = radius, N = approx number of segments)");
					ImGui::Spacing();
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					float const min_widget_width = ImGui::CalcTextSize("R: MMM\nN: MMM").x;
					for (int n = 0; n < 8; n++)
					{
						float const RAD_MIN = 5.0f;
						float const RAD_MAX = 70.0f;
						float const rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

						ImGui::BeginGroup();

						// N is not always exact here due to how PathArcTo() function work internally
						ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

						float const canvas_width = min_widget_width > rad * 2.0f ? min_widget_width : rad * 2.0f;
						float const offset_x     = floorf(canvas_width * 0.5f);
						float const offset_y     = floorf(RAD_MAX);

						ImVec2 const p1 = ImGui::GetCursorScreenPos();
						draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
						ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

						/*
						ImVec2 const p2 = ImGui::GetCursorScreenPos();
						draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
						ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
						*/

						ImGui::EndGroup();
						ImGui::SameLine();
					}
					ImGui::EndTooltip();
				}
				ImGui::SameLine();
				HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

				ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
				ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); HelpMarker("Additional alpha multiplier for disabled items (multiply over current value of Alpha).");
				ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::PopItemWidth();
	}
	ImGui::End();
}

} // namespace Editor