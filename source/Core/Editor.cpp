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

	enum ImGuiDataType_
	{
		ImGuiDataType_S8,       // signed char / char (with sensible compilers)
		ImGuiDataType_U8,       // unsigned char
		ImGuiDataType_S16,      // short
		ImGuiDataType_U16,      // unsigned short
		ImGuiDataType_S32,      // int
		ImGuiDataType_U32,      // unsigned int
		ImGuiDataType_S64,      // long long / __int64
		ImGuiDataType_U64,      // unsigned long long / unsigned __int64
		ImGuiDataType_Float,    // float
		ImGuiDataType_Double,   // double
		ImGuiDataType_Bool,     // bool (provided for user convenience, not supported by scalar widgets)
		ImGuiDataType_COUNT
	};

    std::string line;
	std::string_view name = "";
	void* pvar = nullptr;
	// int* pInt = nullptr;
	// float* pFloat = nullptr;
	ImGuiDataVarInfo var_info{ImGuiDataType_COUNT};
	if (!std::getline(file, line)) return false;
    do {
		if (var_info.Type == ImGuiDataType_COUNT) {
			size_t name_start = line.find_first_of("[");
			if (name_start == std::string::npos) continue;
			size_t name_end = line.find_first_of("]", name_start + 1);
			if (name_end == std::string::npos) continue;
			name = std::string_view(line.begin() + name_start + 1, line.begin() + name_end);
			if (name.empty()) continue;

			for (auto i = 0; i != ImGuiStyleVar_COUNT; i++) {
				if (name == GetStyleVarName(i)) {
					var_info = *ImGui::GetStyleVarInfo(i);
					pvar = var_info.GetVarPtr((void*)&style);
					std::getline(file, line);
					continue;
				}
			}
			bool is_extra = false;
			if (name == "WindowMenuButtonPosition")   {var_info = {ImGuiDataType_Float, 1}; pvar = &style.WindowMenuButtonPosition;   is_extra = true;}
			if (name == "TouchExtraPadding")          {var_info = {ImGuiDataType_Float, 2}; pvar = &style.TouchExtraPadding;          is_extra = true;}
			if (name == "ColumnsMinSpacing")          {var_info = {ImGuiDataType_Float, 1}; pvar = &style.ColumnsMinSpacing;          is_extra = true;}
			if (name == "LogSliderDeadzone")          {var_info = {ImGuiDataType_Float, 1}; pvar = &style.LogSliderDeadzone;          is_extra = true;}
			if (name == "TabMinWidthForCloseButton")  {var_info = {ImGuiDataType_Float, 1}; pvar = &style.TabMinWidthForCloseButton;  is_extra = true;}
			if (name == "ColorButtonPosition")        {var_info = {ImGuiDataType_S32,   1}; pvar = &style.ColorButtonPosition;        is_extra = true;}
			if (name == "DisplayWindowPadding")       {var_info = {ImGuiDataType_Float, 2}; pvar = &style.DisplayWindowPadding;       is_extra = true;}
			if (name == "DisplaySafeAreaPadding")     {var_info = {ImGuiDataType_Float, 2}; pvar = &style.DisplaySafeAreaPadding;     is_extra = true;}
			if (name == "MouseCursorScale")           {var_info = {ImGuiDataType_Float, 1}; pvar = &style.MouseCursorScale;           is_extra = true;}
			if (name == "AntiAliasedLines")           {var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedLines;           is_extra = true;}
			if (name == "AntiAliasedLinesUseTex")     {var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedLinesUseTex;     is_extra = true;}
			if (name == "AntiAliasedFill")            {var_info = {ImGuiDataType_Bool,  1}; pvar = &style.AntiAliasedFill;            is_extra = true;}
			if (name == "CurveTessellationTol")       {var_info = {ImGuiDataType_Float, 1}; pvar = &style.CurveTessellationTol;       is_extra = true;}
			if (name == "CircleTessellationMaxError") {var_info = {ImGuiDataType_Float, 1}; pvar = &style.CircleTessellationMaxError; is_extra = true;}
			if (name == "HoverStationaryDelay")       {var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverStationaryDelay;       is_extra = true;}
			if (name == "HoverDelayShort")            {var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverDelayShort;            is_extra = true;}
			if (name == "HoverDelayNormal")           {var_info = {ImGuiDataType_Float, 1}; pvar = &style.HoverDelayNormal;           is_extra = true;}
			if (name == "HoverFlagsForTooltipMouse")  {var_info = {ImGuiDataType_S32,   1}; pvar = &style.HoverFlagsForTooltipMouse;  is_extra = true;}
			if (name == "HoverFlagsForTooltipNav")    {var_info = {ImGuiDataType_S32,   1}; pvar = &style.HoverFlagsForTooltipNav;    is_extra = true;}
			if (is_extra) {
				std::getline(file, line);
				continue; 
			}
			for (int i = 0; i < ImGuiCol_COUNT; i++) {
				if (name == ImGui::GetStyleColorName(i)) {
					var_info.Type = ImGuiDataType_Float;
					var_info.Count = 4;
					pvar = &style.Colors[i];
				}
			}
			std::getline(file, line);
			continue;
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
				LOG_WARN("Parameter {} skipped (unsupported count {})", name, var_info.Count);
			}
			var_info.Type = ImGuiDataType_COUNT;
			std::getline(file, line);
			continue;
		}
        
    } while (std::getline(file, line));

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
			LoadStyle("style.ini", ImGui::GetStyle());
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
	case ImGuiStyleVar_Alpha:                       return "Alpha";                       // float     Alpha
	case ImGuiStyleVar_DisabledAlpha:               return "DisabledAlpha";               // float     DisabledAlpha
	case ImGuiStyleVar_WindowPadding:               return "WindowPadding";               // ImVec2    WindowPadding
	case ImGuiStyleVar_WindowRounding:              return "WindowRounding";              // float     WindowRounding
	case ImGuiStyleVar_WindowBorderSize:            return "WindowBorderSize";            // float     WindowBorderSize
	case ImGuiStyleVar_WindowMinSize:               return "WindowMinSize";               // ImVec2    WindowMinSize
	case ImGuiStyleVar_WindowTitleAlign:            return "WindowTitleAlign";            // ImVec2    WindowTitleAlign
	case ImGuiStyleVar_ChildRounding:               return "ChildRounding";               // float     ChildRounding
	case ImGuiStyleVar_ChildBorderSize:             return "ChildBorderSize";             // float     ChildBorderSize
	case ImGuiStyleVar_PopupRounding:               return "PopupRounding";               // float     PopupRounding
	case ImGuiStyleVar_PopupBorderSize:             return "PopupBorderSize";             // float     PopupBorderSize
	case ImGuiStyleVar_FramePadding:                return "FramePadding";                // ImVec2    FramePadding
	case ImGuiStyleVar_FrameRounding:               return "FrameRounding";               // float     FrameRounding
	case ImGuiStyleVar_FrameBorderSize:             return "FrameBorderSize";             // float     FrameBorderSize
	case ImGuiStyleVar_ItemSpacing:                 return "ItemSpacing";                 // ImVec2    ItemSpacing
	case ImGuiStyleVar_ItemInnerSpacing:            return "ItemInnerSpacing";            // ImVec2    ItemInnerSpacing
	case ImGuiStyleVar_IndentSpacing:               return "IndentSpacing";               // float     IndentSpacing
	case ImGuiStyleVar_CellPadding:                 return "CellPadding";                 // ImVec2    CellPadding
	case ImGuiStyleVar_ScrollbarSize:               return "ScrollbarSize";               // float     ScrollbarSize
	case ImGuiStyleVar_ScrollbarRounding:           return "ScrollbarRounding";           // float     ScrollbarRounding
	case ImGuiStyleVar_GrabMinSize:                 return "GrabMinSize";                 // float     GrabMinSize
	case ImGuiStyleVar_GrabRounding:                return "GrabRounding";                // float     GrabRounding
	case ImGuiStyleVar_TabRounding:                 return "TabRounding";                 // float     TabRounding
	case ImGuiStyleVar_TabBorderSize:               return "TabBorderSize";               // float     TabBorderSize
	case ImGuiStyleVar_TabBarBorderSize:            return "TabBarBorderSize";            // float     TabBarBorderSize
	case ImGuiStyleVar_TabBarOverlineSize:          return "TabBarOverlineSize";          // float     TabBarOverlineSize
	case ImGuiStyleVar_TableAngledHeadersAngle:     return "TableAngledHeadersAngle";     // float     TableAngledHeadersAngle
	case ImGuiStyleVar_TableAngledHeadersTextAlign: return "TableAngledHeadersTextAlign"; // ImVec2    TableAngledHeadersTextAlign
	case ImGuiStyleVar_ButtonTextAlign:             return "ButtonTextAlign";             // ImVec2    ButtonTextAlign
	case ImGuiStyleVar_SelectableTextAlign:         return "SelectableTextAlign";         // ImVec2    SelectableTextAlign
	case ImGuiStyleVar_SeparatorTextBorderSize:     return "SeparatorTextBorderSize";     // float     SeparatorTextBorderSize
	case ImGuiStyleVar_SeparatorTextAlign:          return "SeparatorTextAlign";          // ImVec2    SeparatorTextAlign
	case ImGuiStyleVar_SeparatorTextPadding:        return "SeparatorTextPadding";        // ImVec2    SeparatorTextPadding
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