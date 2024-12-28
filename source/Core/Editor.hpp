#pragma once
#include <imgui/imgui.h>
#include "Objects.hpp"

struct Editor {
	void Setup();
	void Finish();
    void BeginFrame();
	void Draw(Objects::Camera& camera);
    ImDrawData* EndFrame();
};