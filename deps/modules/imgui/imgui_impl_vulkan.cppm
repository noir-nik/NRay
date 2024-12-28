#ifdef USE_MODULES
module;
#else
#pragma once
#endif

#include <imgui_impl_vulkan.h>

// NOLINTBEGIN(misc-unused-using-decls)
#ifdef USE_MODULES
export module imgui_impl_vulkan;

export using ::ImGui_ImplVulkan_InitInfo;
export using ::ImGui_ImplVulkan_Init;
export using ::ImGui_ImplVulkan_Shutdown;
export using ::ImGui_ImplVulkan_NewFrame;
export using ::ImGui_ImplVulkan_AddTexture;
export using ::ImGui_ImplVulkan_RemoveTexture;
export using ::ImGui_ImplVulkan_RenderDrawData;

#endif
// NOLINTEND(misc-unused-using-decls)