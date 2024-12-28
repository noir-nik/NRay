#pragma once

namespace UI {

struct Context {
	void Init();
	void Destroy();
	void SetCurrent(); // Thanks ImGui
private:
	void* context = nullptr;
};


void Init();
void Destroy();

bool SaveStyle();
bool LoadStyle();
}