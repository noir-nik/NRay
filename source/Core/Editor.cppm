#ifdef USE_MODULES
export module Editor;
#define _EDITOR_EXPORT export
import Objects;
import Runtime;
#else
#pragma once
#define _EDITOR_EXPORT
#include "Objects.cppm"
#include "Runtime.cppm"
#endif

_EDITOR_EXPORT
struct Editor {
	using UiDrawData = void;
	using UiStyle = void;
	void Setup();
	void Finish();
    void BeginFrame();
	void Draw(Runtime::Context& rtc);
    UiDrawData* EndFrame();
	
	UiStyle* GetStyle();
};