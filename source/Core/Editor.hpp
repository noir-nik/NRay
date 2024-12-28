#pragma once
#include <imgui/imgui.h>

struct Editor {

	void Setup();
	void Finish();
    void BeginFrame();
	void Draw();
    ImDrawData* EndFrame();


};