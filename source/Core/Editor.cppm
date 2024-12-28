#ifdef USE_MODULES
export module Editor;
#define _EDITOR_EXPORT export
import Objects;
import RuntimeContext;
#else
#pragma once
#define _EDITOR_EXPORT
#include "Objects.cppm"
#include "RuntimeContext.cppm"
#endif

_EDITOR_EXPORT
struct Editor {
	using UiDrawData = void;
	using UiStyle = void;
	void Setup();
	void Finish();
    void BeginFrame();
	void Draw(RuntimeContext& rtc);
    UiDrawData* EndFrame();
	
	UiStyle* GetStyle();
};