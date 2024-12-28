#ifdef USE_MODULES
export module Editor;
#define _EDITOR_EXPORT export
import Runtime;
#else
#pragma once
#define _EDITOR_EXPORT
#include "Runtime.cppm"
#endif

_EDITOR_EXPORT
namespace Editor {
	using UiDrawData = void;
	using UiStyle = void;
	void Setup();
	void Finish();
    void BeginFrame();
	void Draw(Runtime::Context& rtc);
    UiDrawData* EndFrame();
	UiStyle* GetStyle();
};