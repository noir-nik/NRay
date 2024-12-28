#ifdef USE_MODULES
export module UI;
#define _UI_EXPORT export
#else
#pragma once
#define _UI_EXPORT

#endif

_UI_EXPORT
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