#ifdef USE_MODULES
export module Editor;
#define _EDITOR_EXPORT export
import Objects;
import imgui;
#else
#pragma once
#define _EDITOR_EXPORT
#include <imgui/imgui.h>
#include "Objects.cppm"
#endif

_EDITOR_EXPORT
struct Editor {
	void Setup();
	void Finish();
    void BeginFrame();
	void Draw(Objects::Camera& camera);
    ImDrawData* EndFrame();
};