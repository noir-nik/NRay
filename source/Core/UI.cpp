#ifdef USE_MODULES
module UI;
import VulkanBackend;
import Log;
import stl;
import imgui;
#else
#include "UI.cppm"
#include "VulkanBackend.cppm"
#include "imgui.cppm"
#include "Log.cppm"

#include <filesystem>
#endif


namespace UI {

ImFontAtlas* sharedFontAtlas = nullptr;
ImFont* defaultFont = nullptr;
ImGuiContext* setupContext = nullptr;

void Init(){
	IMGUI_CheckVersion();
	setupContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	sharedFontAtlas = io.Fonts;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// io.IniFilename = nullptr;

	// io.Fonts->AddFontDefault();
	const float fontSize = 15.0f;
	for (const auto& entry : std::filesystem::directory_iterator("assets/fonts")) {
		if (entry.path().extension() == ".ttf" || entry.path().extension() == ".otf") {
			// auto font = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), fontSize);
			// if (font && entry.path().filename() == "InterVariable.ttf") {
			// 	io.FontDefault = font;
			// }
			if (entry.path().filename() == "InterVariable.ttf") {
				defaultFont = sharedFontAtlas->AddFontFromFileTTF(entry.path().string().c_str(), fontSize);
				if (defaultFont == nullptr) {
					LOG_WARN("Failed to load font: {}", entry.path().string());
					defaultFont = sharedFontAtlas->AddFontDefault();
				}
			}
		}
	}
	io.FontDefault = defaultFont;

}

void Destroy() {
	ImGui::SetCurrentContext(setupContext);
	// vkw::ImGuiShutdown();
	ImGui::DestroyContext(setupContext);
}


void Context::Init(void* imguiStyle) {
	context = ImGui::CreateContext(sharedFontAtlas);
	SetCurrent();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.FontDefault = defaultFont;

	if (imguiStyle != nullptr){
		ImGui::GetStyle() = *static_cast<ImGuiStyle*>(imguiStyle);
	}
}

void Context::SetCurrent(){
	ImGui::SetCurrentContext(static_cast<ImGuiContext*>(context));
}

void Context::Destroy() {
	SetCurrent();
	vkw::ImGuiShutdown();
	ImGui::DestroyContext(static_cast<ImGuiContext*>(context));
}



} // namespace UI