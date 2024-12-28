#include "Editor.hpp"
#include "VulkanBase.hpp"
#include "Base.hpp"
#include <cstdio>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string_view>

#include <fstream>

struct EditorContext {
	
	void MainMenu();
	void RenderUI();
	void StyleWindow(); 
	
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

	if (style.Alpha != defaultStyle.Alpha) fprintf(f, "[Alpha]\n%1.3f\n", style.Alpha);

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

	if (style.WindowMenuButtonPosition != defaultStyle.WindowMenuButtonPosition)      fprintf(f, "[WindowMenuButtonPosition]\n%1.3f\n", style.WindowMenuButtonPosition);
    if (style.TouchExtraPadding[0] != defaultStyle.TouchExtraPadding[0]
		|| style.TouchExtraPadding[1] != defaultStyle.TouchExtraPadding[1])           fprintf(f, "[TouchExtraPadding]\n%1.3f %1.3f\n", style.TouchExtraPadding[0], style.TouchExtraPadding[1]);
    if (style.ColumnsMinSpacing != defaultStyle.ColumnsMinSpacing)                    fprintf(f, "[ColumnsMinSpacing]\n%1.3f\n", style.ColumnsMinSpacing);
    if (style.LogSliderDeadzone != defaultStyle.LogSliderDeadzone)                    fprintf(f, "[LogSliderDeadzone]\n%1.3f\n", style.LogSliderDeadzone);
    if (style.TabMinWidthForCloseButton != defaultStyle.TabMinWidthForCloseButton)    fprintf(f, "[TabMinWidthForCloseButton]\n%1.3f\n", style.TabMinWidthForCloseButton);
    if (style.ColorButtonPosition != defaultStyle.ColorButtonPosition)                fprintf(f, "[ColorButtonPosition]\n%d\n", style.ColorButtonPosition);
    if (style.DisplayWindowPadding[0] != defaultStyle.DisplayWindowPadding[0]
		|| style.DisplayWindowPadding[1] != defaultStyle.DisplayWindowPadding[1])     fprintf(f, "[DisplayWindowPadding]\n%1.3f %1.3f\n", style.DisplayWindowPadding[0], style.DisplayWindowPadding[1]);
    if (style.DisplaySafeAreaPadding[0] != defaultStyle.DisplaySafeAreaPadding[0]
		|| style.DisplaySafeAreaPadding[1] != defaultStyle.DisplaySafeAreaPadding[1]) fprintf(f, "[DisplaySafeAreaPadding]\n%1.3f %1.3f\n", style.DisplaySafeAreaPadding[0], style.DisplaySafeAreaPadding[1]);
    if (style.MouseCursorScale != defaultStyle.MouseCursorScale)                      fprintf(f, "[MouseCursorScale]\n%1.3f\n", style.MouseCursorScale);
    if (style.AntiAliasedLines != defaultStyle.AntiAliasedLines)                      fprintf(f, "[AntiAliasedLines]\n%d\n", style.AntiAliasedLines);
    if (style.AntiAliasedLinesUseTex != defaultStyle.AntiAliasedLinesUseTex)          fprintf(f, "[AntiAliasedLinesUseTex]\n%d\n", style.AntiAliasedLinesUseTex);
    if (style.AntiAliasedFill != defaultStyle.AntiAliasedFill)                        fprintf(f, "[AntiAliasedFill]\n%d\n", style.AntiAliasedFill);
    if (style.CurveTessellationTol != defaultStyle.CurveTessellationTol)              fprintf(f, "[CurveTessellationTol]\n%1.3f\n", style.CurveTessellationTol);
    if (style.CircleTessellationMaxError != defaultStyle.CircleTessellationMaxError)  fprintf(f, "[CircleTessellationMaxError]\n%1.3f\n", style.CircleTessellationMaxError);
    if (style.HoverStationaryDelay != defaultStyle.HoverStationaryDelay)              fprintf(f, "[HoverStationaryDelay]\n%1.3f\n", style.HoverStationaryDelay);
    if (style.HoverDelayShort != defaultStyle.HoverDelayShort)                        fprintf(f, "[HoverDelayShort]\n%1.3f\n", style.HoverDelayShort);
    if (style.HoverDelayNormal != defaultStyle.HoverDelayNormal)                      fprintf(f, "[HoverDelayNormal]\n%1.3f\n", style.HoverDelayNormal);
    if (style.HoverFlagsForTooltipMouse != defaultStyle.HoverFlagsForTooltipMouse)    fprintf(f, "[HoverFlagsForTooltipMouse]\n%d\n", style.HoverFlagsForTooltipMouse);
    if (style.HoverFlagsForTooltipNav != defaultStyle.HoverFlagsForTooltipNav)        fprintf(f, "[HoverFlagsForTooltipNav]\n%d\n", style.HoverFlagsForTooltipNav);
		
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
		printf("line: %s\n", line.data());
		if (var_info.Type == ImGuiDataType_COUNT) {
			bool found = 0;
			size_t name_start = line.find_first_of("[");
			if (name_start == std::string::npos) continue;
			size_t name_end = line.find_first_of("]", name_start + 1);
			if (name_end == std::string::npos) continue;
			name = std::string_view(line.begin() + name_start + 1, line.begin() + name_end);
			if (name.empty()) continue;
			printf("name: %s\n", name.data());

			for (auto i = 0; i != ImGuiStyleVar_COUNT; i++) {
				if (name == GetStyleVarName(i)) {
					var_info = *ImGui::GetStyleVarInfo(i);
					pvar = var_info.GetVarPtr((void*)&style);
					found = 1;
					break;
				}
			}
			if (found) continue;
			if (name == "WindowMenuButtonPosition")   {var_info = {ImGuiDataType_Float, 1}; pvar = &style.WindowMenuButtonPosition;   found = true;}
			if (name == "TouchExtraPadding")          {var_info = {ImGuiDataType_Float, 2}; pvar = &style.TouchExtraPadding;          found = true;}
			if (name == "ColumnsMinSpacing")          {var_info = {ImGuiDataType_Float, 1}; pvar = &style.ColumnsMinSpacing;          found = true;}
			if (name == "LogSliderDeadzone")          {var_info = {ImGuiDataType_Float, 1}; pvar = &style.LogSliderDeadzone;          found = true;}
			if (name == "TabMinWidthForCloseButton")  {var_info = {ImGuiDataType_Float, 1}; pvar = &style.TabMinWidthForCloseButton;  found = true;}
			if (name == "ColorButtonPosition")        {var_info = {ImGuiDataType_S32,   1}; pvar = &style.ColorButtonPosition;        found = true;}
			if (name == "DisplayWindowPadding")       {var_info = {ImGuiDataType_Float, 2}; pvar = &style.DisplayWindowPadding;       found = true;}
			if (name == "DisplaySafeAreaPadding")     {var_info = {ImGuiDataType_Float, 2}; pvar = &style.DisplaySafeAreaPadding;     found = true;}
			if (name == "MouseCursorScale")           {var_info = {ImGuiDataType_Float, 1}; pvar = &style.MouseCursorScale;           found = true;}
			if (name == "AntiAliasedLines")           {var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedLines;           found = true;}
			if (name == "AntiAliasedLinesUseTex")     {var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedLinesUseTex;     found = true;}
			if (name == "AntiAliasedFill")            {var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedFill;            found = true;}
			if (name == "CurveTessellationTol")       {var_info = {ImGuiDataType_Float, 1}; pvar = &style.CurveTessellationTol;       found = true;}
			if (name == "CircleTessellationMaxError") {var_info = {ImGuiDataType_Float, 1}; pvar = &style.CircleTessellationMaxError; found = true;}
			if (name == "HoverStationaryDelay")       {var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverStationaryDelay;       found = true;}
			if (name == "HoverDelayShort")            {var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverDelayShort;            found = true;}
			if (name == "HoverDelayNormal")           {var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverDelayNormal;           found = true;}
			if (name == "HoverFlagsForTooltipMouse")  {var_info = {ImGuiDataType_S32,   1}; pvar = &style.HoverFlagsForTooltipMouse;  found = true;}
			if (name == "HoverFlagsForTooltipNav")    {var_info = {ImGuiDataType_S32,   1}; pvar = &style.HoverFlagsForTooltipNav;    found = true;}
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
			switch (var_info.Type) {
			case ImGuiDataType_Bool:{
				int tmp;
				if (sscanf(line.c_str(), "%d", &tmp) == 1) {
					auto p = (bool*)pvar;
					*p = tmp;
					success = true;
				}
			} break;
			case ImGuiDataType_S32: {
				int tmp;
				if (sscanf(line.c_str(), "%d", &tmp) == 1) {
					auto p = (int*)pvar;
					*p = tmp;
					success = true;
				}
			} break;
			case ImGuiDataType_Float: {
				float x, y, z, w;
				auto p = (float*)pvar;
				switch (var_info.Count) {
				case 1:
					if (sscanf(line.c_str(), "%f", &x) == 1) {
						p[0] = x;
						success = true;
					}
					break;
				case 2:
					if (sscanf(line.c_str(), "%f %f", &x, &y) == 2) {
						p[0] = x; p[1] = y;
						success = true;
					}
					break;
				case 3:
					if (sscanf(line.c_str(), "%f %f %f", &x, &y, &z) == 3) {
						p[0] = x; p[1] = y; p[2] = z;
						success = true;
					}
					break;
				case 4:
					if (sscanf(line.c_str(), "%f %f %f %f", &x, &y, &z, &w) == 4) {
						p[0] = x; p[1] = y; p[2] = z; p[3] = w;
						success = true;
					}
					break;
				default:
					break;
				}
			}
			}
			if (!success) {
				LOG_WARN("Invalid value for parameter [{}]: \"{}\", expected {} value(s), skipping this parameter", name, line, var_info.Count);
			}
			var_info.Type = ImGuiDataType_COUNT;
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
		ImGui::MenuItem("Style Selector", "", &ShowStyleSelector);
		ImGui::MenuItem("Font Selector", "", &ShowFontSelector);
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
		ImGui::Begin("Dear ImGui Style Editor", &ShowStyleEditor);
		ImGui::ShowStyleEditor(&style);
		ImGui::End();
	}
	if (ShowStyleSelector){
		ImGui::Begin("Style Selector", &ShowStyleSelector);
		ImGui::ShowStyleSelector("Colors##Selector");
		ImGui::End();
	}
	if (ShowFontSelector){
		ImGui::Begin("Font Selector", &ShowFontSelector);
		ImGui::ShowFontSelector("Fonts##Selector");
		ImGui::End();
	}
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
			SaveStyle("style.ini", ImGui::GetStyle());
		}
		if (ImGui::Button("Load Style")) {
			if (LoadStyle("style.ini", ImGui::GetStyle())) {
				style = ImGui::GetStyle();
			} else {
				ImGui::GetStyle() = style;
			}
		}
	}

	ImGui::End();
}


void Editor::Draw() {
	ImGui::ShowDemoWindow();
	ctx.MainMenu();
	ctx.StyleWindow();
	// ctx.RenderUI();
}

void Editor::BeginFrame() {
	vkw::BeginImGui();
	ImGui::NewFrame();
}

ImDrawData* Editor::EndFrame() {
	ImGui::Render();
	return ImGui::GetDrawData();
}

void Editor::Setup(){
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ctx.style = ImGui::GetStyle();
	ctx.defaultStyle = ctx.style;

	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable

	// io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	// io.BackendFlags &= ~ImGuiBackendFlags_HasMouseCursors;

	// io.Fonts->AddFontDefault();
	const float fontSize = 15.0f;
	for (const auto& entry : std::filesystem::directory_iterator("bin")) {
		if (entry.path().extension() == ".ttf" || entry.path().extension() == ".otf") {
			// auto font = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), fontSize);
			// if (font && entry.path().filename() == "InterVariable.ttf") {
			// 	io.FontDefault = font;
			// }
			if (entry.path().filename() == "InterVariable.ttf") {
				auto font = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), fontSize);
				io.FontDefault = font;
			}
		}
	}
}

void Editor::Finish() {
	ImGui::DestroyContext();
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