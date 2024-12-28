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
	void Setup(SceneGraph* sceneGraph);
	void SetupWindow(Runtime::WindowData& windowData);
	void Finish();
    void BeginFrame();
	void Draw(Runtime::Data const& data);
    UiDrawData* EndFrame();
	UiStyle* GetStyle();
};