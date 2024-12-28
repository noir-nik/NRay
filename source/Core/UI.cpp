#include <filesystem>
#include <imgui.h>

#include "Base.hpp"
#include "VulkanBase.hpp"
#include "UI.hpp"


namespace UI {

ImFontAtlas* sharedFontAtlas = nullptr;
ImFont* defaultFont = nullptr;
ImGuiContext* setupContext = nullptr;

void Init(){
	IMGUI_CHECKVERSION();
	setupContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	sharedFontAtlas = io.Fonts;
	// io.ConfigFlags |= ;
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

	
}

void Destroy() {
	ImGui::SetCurrentContext(setupContext);
	ImGui::DestroyContext(setupContext);
}


void Context::Init() {
	context = ImGui::CreateContext(sharedFontAtlas);
	auto& io = ImGui::GetIO();
	io.FontDefault = defaultFont;
	SetCurrent();
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