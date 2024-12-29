#ifdef USE_MODULES
module;
#else
#pragma once
#endif

#include <imgui_impl_glfw.h>

// NOLINTBEGIN(misc-unused-using-decls)
#ifdef USE_MODULES
export module imgui_impl_glfw;

export using ::ImGui_ImplGlfw_NewFrame;
export using ::ImGui_ImplGlfw_InitForVulkan;
export using ::ImGui_ImplGlfw_Shutdown;

#endif
// NOLINTEND(misc-unused-using-decls)