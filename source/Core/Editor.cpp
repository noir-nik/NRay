#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.hpp"
#endif
#include "Editor.hpp"
#include "VulkanBase.hpp"
#include "Base.hpp"
#include <UI.hpp>
#include <cstdio>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string_view>

#include <fstream>

#define STYLE_PATH "assets/"

struct EditorContext {
	
	void MainMenu();
	void RenderUI();
	void StyleWindow(); 
	void StyleEditor();

	void DebugWindow(Objects::Camera& camera);
	
	bool SaveStyle(const char* filename,const ImGuiStyle& style);
	bool LoadStyle(const char* filename,ImGuiStyle& style);

	bool ShowStyleEditor = false;
	bool ShowStyleSelector = false;
	bool ShowFontSelector = false;

	ImGuiStyle defaultStyle;
	ImGuiStyle style;
	// ImGuiIO& io;

	inline static const char* GetStyleVarName(ImGuiStyleVar idx);
} ctx;

bool EditorContext::SaveStyle(const char* filename, const ImGuiStyle& style) {
	// Write .style file
	FILE* f = fopen(filename, "wt");
	if (!f)  return false;

	for (auto i = 0; i != ImGuiStyleVar_COUNT; i++) {
		const ImGuiDataVarInfo* var_info = ImGui::GetStyleVarInfo(i);
		if (var_info->Type != ImGuiDataType_Float) {DEBUG_ASSERT(0, "Invalid type"); continue;}
		float* pvar = (float*)var_info->GetVarPtr((void*)&style);
		float* pvarDefault = (float*)var_info->GetVarPtr((void*)&defaultStyle);
		if (var_info->Count == 1) {
			if (*pvar != *pvarDefault) fprintf(f, "[%s]\n%1.3f\n", GetStyleVarName(i), *pvar);
		} else {
			if (pvar[0] != pvarDefault[0] || pvar[1] != pvarDefault[1]) {
				fprintf(f, "[%s]\n%1.3f %1.3f\n", GetStyleVarName(i), pvar[0], pvar[1]);
			}
		}
	}

	if (style.WindowMenuButtonPosition != defaultStyle.WindowMenuButtonPosition)      fprintf(f, "[WindowMenuButtonPosition]\n   %1.3f\n",       style.WindowMenuButtonPosition);
    if (style.TouchExtraPadding[0] != defaultStyle.TouchExtraPadding[0]
		|| style.TouchExtraPadding[1] != defaultStyle.TouchExtraPadding[1])           fprintf(f, "[TouchExtraPadding]\n          %1.3f %1.3f\n", style.TouchExtraPadding[0], style.TouchExtraPadding[1]);
    if (style.ColumnsMinSpacing != defaultStyle.ColumnsMinSpacing)                    fprintf(f, "[ColumnsMinSpacing]\n          %1.3f\n",       style.ColumnsMinSpacing);
    if (style.LogSliderDeadzone != defaultStyle.LogSliderDeadzone)                    fprintf(f, "[LogSliderDeadzone]\n          %1.3f\n",       style.LogSliderDeadzone);
    if (style.TabMinWidthForCloseButton != defaultStyle.TabMinWidthForCloseButton)    fprintf(f, "[TabMinWidthForCloseButton]\n  %1.3f\n",       style.TabMinWidthForCloseButton);
    if (style.ColorButtonPosition != defaultStyle.ColorButtonPosition)                fprintf(f, "[ColorButtonPosition]\n        %d\n",          style.ColorButtonPosition);
    if (style.DisplayWindowPadding[0] != defaultStyle.DisplayWindowPadding[0]
		|| style.DisplayWindowPadding[1] != defaultStyle.DisplayWindowPadding[1])     fprintf(f, "[DisplayWindowPadding]\n       %1.3f %1.3f\n", style.DisplayWindowPadding[0], style.DisplayWindowPadding[1]);
    if (style.DisplaySafeAreaPadding[0] != defaultStyle.DisplaySafeAreaPadding[0]
		|| style.DisplaySafeAreaPadding[1] != defaultStyle.DisplaySafeAreaPadding[1]) fprintf(f, "[DisplaySafeAreaPadding]\n     %1.3f %1.3f\n", style.DisplaySafeAreaPadding[0], style.DisplaySafeAreaPadding[1]);
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
		const ImVec4& c = style.Colors[i];
		if (c.x != defaultStyle.Colors[i].x || c.y != defaultStyle.Colors[i].y || c.z != defaultStyle.Colors[i].z || c.w != defaultStyle.Colors[i].w) {
			fprintf(f, "[%s]\n%1.3f %1.3f %1.3f %1.3f\n", ImGui::GetStyleColorName(i), c.x,c.y,c.z,c.w);
		}
	}

	fprintf(f,"\n");
	fclose(f);
	
	return true;
}

bool EditorContext::LoadStyle(const char* filename, ImGuiStyle& style) {
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
				if (name == GetStyleVarName(i)) {
					var_info = *ImGui::GetStyleVarInfo(i);
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
					auto num_scanned = sscanf(line.c_str(), "%f %f %f %f", &x, &y, &z, &w);
					if (num_scanned == var_info.Count) {
						switch (var_info.Count) {
							case 4: p[3] = w; 
							case 3: p[2] = z; 
							case 2: p[1] = y; 
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


void EditorContext::MainMenu() {
	if (!ImGui::BeginMainMenuBar()) return;
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
		ImGui::MenuItem("Style Editor", "", &ShowStyleEditor);
		// ImGui::MenuItem("Style Selector", "", &ShowStyleSelector);
		// ImGui::MenuItem("Font Selector", "", &ShowFontSelector);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help")) {
		ImGui::MenuItem("Documentation");
		ImGui::MenuItem("About");
		ImGui::EndMenu();
	}
	if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_None)){
		if (ImGui::BeginTabItem("View3D")) {
			// stuff in tab here
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
	}
	ImGui::EndMainMenuBar();
	if (ShowStyleEditor){
		ImGui::Begin("Style Editor", &ShowStyleEditor);
		StyleEditor();
		ImGui::End();
	}
	// if (ShowStyleSelector){
	// 	ImGui::Begin("Style Selector", &ShowStyleSelector);
	// 	ImGui::ShowStyleSelector("Colors##Selector");
	// 	ImGui::End();
	// }
	// if (ShowFontSelector){
	// 	ImGui::Begin("Font Selector", &ShowFontSelector);
	// 	ImGui::ShowFontSelector("Fonts##Selector");
	// 	ImGui::End();
	// }
}


void EditorContext::RenderUI() {
	ImGui::Begin("Scene Panel", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	ImGui::Text("Scene Objects:");
	ImGui::Separator();

	static const char* sceneObjects[] = { "Camera", "Light", "Cube", "Sphere", "Plane" };
	for (int i = 0; i < IM_ARRAYSIZE(sceneObjects); i++) {
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

void EditorContext::StyleWindow() {
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
			ShowStyleEditor = true;
		}
	}

	ImGui::End();
}

void EditorContext::DebugWindow(Objects::Camera& camera) {

	if (ImGui::Begin("Debug Window", nullptr)) {
		ImGui::Text("Camera Focus: (%.2f, %.2f, %.2f)", camera.focus.x, camera.focus.y, camera.focus.z);

		// static float position[3] = { 0.0f, 2.0f, -30.0f };
		// static float rotation[3] = { 0.0f, 0.0f, 0.0f };
		static float cameraUp[3] = { 0.0f, 1.0f, 0.0f };

		
		Lmath::float3 position;
		Lmath::float3 rotation;
		Lmath::float3 scale;
		camera.view.inverse().decompose(position, rotation, scale);

		// ImGui::SliderFloat3("Camera Position", position.M, -50.0f, 50.0f);
		// ImGui::SliderFloat3("Camera Focus", rotation.M, -50.0f, 50.0f);
		// ImGui::SliderFloat3("Camera Up", cameraUp, -1.0f, 1.0f);

		// camera.view = Lmath::inverse4x4(Lmath::lookAt(Lmath::vec3(position[0], position[1], position[2]),
		// 	Lmath::vec3(rotation[0], rotation[1], rotation[2]),
		// 	Lmath::vec3(cameraUp[0], cameraUp[1], cameraUp[2])));

		// camera.view = Lmath::lookAt(Lmath::vec3(position[0], position[1], position[2]),
		// 	Lmath::vec3(rotation[0], rotation[1], rotation[2]),
		// 	Lmath::vec3(cameraUp[0], cameraUp[1], cameraUp[2]));


		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
		ImGui::Text("Camera Rotation: (%.2f, %.2f, %.2f)", rotation.x, rotation.y, rotation.z);
		ImGui::Text("Camera Scale: (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);

		for (int i = 0; i < 4; i++) {
			ImGui::Text("%.2f, %.2f, %.2f, %.2f",
				camera.view[i][0], camera.view[i][1], camera.view[i][2], camera.view[i][3]);
		}
	}

	ImGui::End();
}


void Editor::Draw(Objects::Camera& camera) {

	ImGui::ShowDemoWindow();
	ctx.MainMenu();

	ctx.DebugWindow(camera);

	// ctx.StyleWindow();
	
	// ctx.RenderUI();
}

void Editor::BeginFrame() {
	vkw::ImGuiNewFrame();
	ImGui::NewFrame();
}

ImDrawData* Editor::EndFrame() {
	ImGui::Render();
	return ImGui::GetDrawData();
}

void Editor::Setup(){

	ctx.defaultStyle = ImGui::GetStyle();

	if (ctx.LoadStyle(STYLE_PATH"style.ini", ImGui::GetStyle())) {
		LOG_INFO("Loaded style");
		ctx.style = ImGui::GetStyle();
	} else {
		LOG_WARN("Failed to load style");
		ImGui::GetStyle() = ctx.defaultStyle;
		ctx.style = ctx.defaultStyle;
	}

}


inline const char* EditorContext::GetStyleVarName(ImGuiStyleVar idx) {
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

static void HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void EditorContext::StyleEditor() {
    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    ImGuiStyle& style = ImGui::GetStyle();
	ImGuiStyle& ref = this->style; 
    static ImGuiStyle ref_saved_style;

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

    if (ImGui::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
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
                ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < ImGuiCol_COUNT; i++) {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref.Colors[i], sizeof(ImVec4)) != 0)
                        ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
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

            ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
            ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
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
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fonts"))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
            ImGui::ShowFontAtlas(atlas);

            // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
            // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
            const float MIN_SCALE = 0.3f;
            const float MAX_SCALE = 2.0f;
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
            const bool show_samples = ImGui::IsItemActive();
            if (show_samples)
                ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
            if (show_samples && ImGui::BeginTooltip())
            {
                ImGui::TextUnformatted("(R = radius, N = approx number of segments)");
                ImGui::Spacing();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                const float min_widget_width = ImGui::CalcTextSize("R: MMM\nN: MMM").x;
                for (int n = 0; n < 8; n++)
                {
                    const float RAD_MIN = 5.0f;
                    const float RAD_MAX = 70.0f;
                    const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

                    ImGui::BeginGroup();

                    // N is not always exact here due to how PathArcTo() function work internally
                    ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

                    const float canvas_width = min_widget_width > rad * 2.0f ? min_widget_width : rad * 2.0f;
                    const float offset_x     = floorf(canvas_width * 0.5f);
                    const float offset_y     = floorf(RAD_MAX);

                    const ImVec2 p1 = ImGui::GetCursorScreenPos();
                    draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
                    ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

                    /*
                    const ImVec2 p2 = ImGui::GetCursorScreenPos();
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
