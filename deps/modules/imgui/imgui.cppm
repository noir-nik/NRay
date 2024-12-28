#ifdef USE_MODULES
module;
#define _IMGUI_EXPORT export
#else
#pragma once
#define _IMGUI_EXPORT
#endif

#include <imgui.h>
#include <imgui_internal.h>

// NOLINTBEGIN(misc-unused-using-decls)
#ifdef USE_MODULES
export module imgui;

// typedefs
export using ::ImGuiID;
export using ::ImS8;
export using ::ImU8;
export using ::ImS16;
export using ::ImU16;
export using ::ImS32;
export using ::ImU32;
export using ::ImS64;
export using ::ImU64;

export using ::ImDrawChannel;
export using ::ImDrawCmd;
export using ::ImDrawData;
export using ::ImDrawList;
export using ::ImDrawListSharedData;
export using ::ImDrawListSplitter;
export using ::ImDrawVert;
export using ::ImFont;
export using ::ImFontAtlas;
export using ::ImFontBuilderIO;
export using ::ImFontConfig;
export using ::ImFontGlyph;
export using ::ImFontGlyphRangesBuilder;
export using ::ImColor;
export using ::ImGuiContext;
export using ::ImGuiIO;
export using ::ImGuiInputTextCallbackData;
export using ::ImGuiKeyData;
export using ::ImGuiListClipper;
export using ::ImGuiMultiSelectIO;
export using ::ImGuiOnceUponAFrame;
export using ::ImGuiPayload;
export using ::ImGuiPlatformIO;
export using ::ImGuiPlatformImeData;
export using ::ImGuiPlatformMonitor;
export using ::ImGuiSelectionBasicStorage;
export using ::ImGuiSelectionExternalStorage;
export using ::ImGuiSelectionRequest;
export using ::ImGuiSizeCallbackData;
export using ::ImGuiStorage;
export using ::ImGuiStoragePair;
export using ::ImGuiStyle;
export using ::ImGuiTableSortSpecs;
export using ::ImGuiTableColumnSortSpecs;
export using ::ImGuiTextBuffer;
export using ::ImGuiTextFilter;
export using ::ImGuiViewport;
export using ::ImGuiWindowClass;

export using ::ImGuiCol;
export using ::ImGuiCond;
export using ::ImGuiDataType;
export using ::ImGuiMouseButton;
export using ::ImGuiMouseCursor;
export using ::ImGuiStyleVar;
export using ::ImGuiTableBgTarget;

export using ::ImDrawFlags;
export using ::ImDrawListFlags;
export using ::ImFontAtlasFlags;
export using ::ImGuiBackendFlags;
export using ::ImGuiButtonFlags;
export using ::ImGuiChildFlags;
export using ::ImGuiColorEditFlags;
export using ::ImGuiConfigFlags;
export using ::ImGuiComboFlags;
export using ::ImGuiDockNodeFlags;
export using ::ImGuiDragDropFlags;
export using ::ImGuiFocusedFlags;
export using ::ImGuiHoveredFlags;
export using ::ImGuiInputFlags;
export using ::ImGuiInputTextFlags;
export using ::ImGuiItemFlags;
export using ::ImGuiKeyChord;
export using ::ImGuiPopupFlags;
export using ::ImGuiMultiSelectFlags;
export using ::ImGuiSelectableFlags;
export using ::ImGuiSliderFlags;
export using ::ImGuiTabBarFlags;
export using ::ImGuiTabItemFlags;
export using ::ImGuiTableFlags;
export using ::ImGuiTableColumnFlags;
export using ::ImGuiTableRowFlags;
export using ::ImGuiTreeNodeFlags;
export using ::ImGuiViewportFlags;
export using ::ImGuiWindowFlags;
export using ::ImTextureID;
export using ::ImDrawIdx;
export using ::ImWchar32;
export using ::ImWchar16;
export using ::ImWchar;
export using ::ImWchar;
export using ::ImGuiSelectionUserData;


export using ::ImDrawChannel;
export using ::ImVec2;
export using ::ImVec4;

namespace ImGui
{
    export using ImGui::CreateContext;
    export using ImGui::DestroyContext;
    export using ImGui::GetCurrentContext;
    export using ImGui::SetCurrentContext;
    export using ImGui::GetIO;
    export using ImGui::GetPlatformIO;
    export using ImGui::GetStyle;
    export using ImGui::NewFrame;
    export using ImGui::EndFrame;
    export using ImGui::Render;
    export using ImGui::GetDrawData;
    export using ImGui::ShowDemoWindow;
    export using ImGui::ShowMetricsWindow;
    export using ImGui::ShowDebugLogWindow;
    export using ImGui::ShowIDStackToolWindow;
    export using ImGui::ShowAboutWindow;
    export using ImGui::ShowStyleEditor;
    export using ImGui::ShowStyleSelector;
    export using ImGui::ShowFontSelector;
    export using ImGui::ShowUserGuide;
    export using ImGui::GetVersion;
    export using ImGui::StyleColorsDark;
    export using ImGui::StyleColorsLight;
    export using ImGui::StyleColorsClassic;
    export using ImGui::Begin;
    export using ImGui::End;
    export using ImGui::BeginChild;
    export using ImGui::BeginChild;
    export using ImGui::EndChild;
    export using ImGui::IsWindowAppearing;
    export using ImGui::IsWindowCollapsed;
    export using ImGui::IsWindowFocused;
    export using ImGui::IsWindowHovered;
    export using ImGui::GetWindowDrawList;
    export using ImGui::GetWindowDpiScale;
    export using ImGui::GetWindowPos;
    export using ImGui::GetWindowSize;
    export using ImGui::GetWindowWidth;
    export using ImGui::GetWindowHeight;
    export using ImGui::GetWindowViewport;
    export using ImGui::SetNextWindowPos;
    export using ImGui::SetNextWindowSize;
    export using ImGui::SetNextWindowSizeConstraints;
    export using ImGui::SetNextWindowContentSize;
    export using ImGui::SetNextWindowCollapsed;
    export using ImGui::SetNextWindowFocus;
    export using ImGui::SetNextWindowScroll;
    export using ImGui::SetNextWindowBgAlpha;
    export using ImGui::SetNextWindowViewport;
    export using ImGui::SetWindowPos;
    export using ImGui::SetWindowSize;
    export using ImGui::SetWindowCollapsed;
    export using ImGui::SetWindowFocus;
    export using ImGui::SetWindowFontScale;
    export using ImGui::SetWindowPos;
    export using ImGui::SetWindowSize;
    export using ImGui::SetWindowCollapsed;
    export using ImGui::SetWindowFocus;
    export using ImGui::GetScrollX;
    export using ImGui::GetScrollY;
    export using ImGui::SetScrollX;
    export using ImGui::SetScrollY;
    export using ImGui::GetScrollMaxX;
    export using ImGui::GetScrollMaxY;
    export using ImGui::SetScrollHereX;
    export using ImGui::SetScrollHereY;
    export using ImGui::SetScrollFromPosX;
    export using ImGui::SetScrollFromPosY;
    export using ImGui::PushFont;
    export using ImGui::PopFont;
    export using ImGui::PushStyleColor;
    export using ImGui::PushStyleColor;
    export using ImGui::PopStyleColor;
    export using ImGui::PushStyleVar;
    export using ImGui::PushStyleVar;
    export using ImGui::PushStyleVarX;
    export using ImGui::PushStyleVarY;
    export using ImGui::PopStyleVar;
    export using ImGui::PushItemFlag;
    export using ImGui::PopItemFlag;
    export using ImGui::PushItemWidth;
    export using ImGui::PopItemWidth;
    export using ImGui::SetNextItemWidth;
    export using ImGui::CalcItemWidth;
    export using ImGui::PushTextWrapPos;
    export using ImGui::PopTextWrapPos;
    export using ImGui::GetFont;
    export using ImGui::GetFontSize;
    export using ImGui::GetFontTexUvWhitePixel;
    export using ImGui::GetColorU32;
    export using ImGui::GetColorU32;
    export using ImGui::GetColorU32;
    export using ::ImVec4;
    export using ImGui::GetCursorScreenPos;
    export using ImGui::SetCursorScreenPos;
    export using ImGui::GetContentRegionAvail;
    export using ImGui::GetCursorPos;
    export using ImGui::GetCursorPosX;
    export using ImGui::GetCursorPosY;
    export using ImGui::SetCursorPos;
    export using ImGui::SetCursorPosX;
    export using ImGui::SetCursorPosY;
    export using ImGui::GetCursorStartPos;
    export using ImGui::Separator;
    export using ImGui::SameLine;
    export using ImGui::NewLine;
    export using ImGui::Spacing;
    export using ImGui::Dummy;
    export using ImGui::Indent;
    export using ImGui::Unindent;
    export using ImGui::BeginGroup;
    export using ImGui::EndGroup;
    export using ImGui::AlignTextToFramePadding;
    export using ImGui::GetTextLineHeight;
    export using ImGui::GetTextLineHeightWithSpacing;
    export using ImGui::GetFrameHeight;
    export using ImGui::GetFrameHeightWithSpacing;
    export using ImGui::PushID;
    export using ImGui::PushID;
    export using ImGui::PushID;
    export using ImGui::PushID;
    export using ImGui::PopID;
    export using ImGui::GetID;
    export using ImGui::GetID;
    export using ImGui::GetID;
    export using ImGui::GetID;
    export using ImGui::TextUnformatted;
    export using ImGui::Text;
    export using ImGui::TextV;
    export using ImGui::TextColored;
    export using ImGui::TextColoredV;
    export using ImGui::TextDisabled;
    export using ImGui::TextDisabledV;
    export using ImGui::TextWrapped;
    export using ImGui::TextWrappedV;
    export using ImGui::LabelText;
    export using ImGui::LabelTextV;
    export using ImGui::BulletText;
    export using ImGui::BulletTextV;
    export using ImGui::SeparatorText;
    export using ImGui::Button;
    export using ImGui::SmallButton;
    export using ImGui::InvisibleButton;
    export using ImGui::ArrowButton;
    export using ImGui::Checkbox;
    export using ImGui::CheckboxFlags;
    export using ImGui::CheckboxFlags;
    export using ImGui::RadioButton;
    export using ImGui::RadioButton;
    export using ImGui::ProgressBar;
    export using ImGui::Bullet;
    export using ImGui::TextLink;
    export using ImGui::TextLinkOpenURL;
    export using ImGui::Image;
    export using ImGui::ImageButton;
    export using ImGui::BeginCombo;
    export using ImGui::EndCombo;
    export using ImGui::Combo;
    export using ImGui::Combo;
    export using ImGui::Combo;
    export using ImGui::DragFloat;
    export using ImGui::DragFloat2;
    export using ImGui::DragFloat3;
    export using ImGui::DragFloat4;
    export using ImGui::DragFloatRange2;
    export using ImGui::DragInt;
    export using ImGui::DragInt2;
    export using ImGui::DragInt3;
    export using ImGui::DragInt4;
    export using ImGui::DragIntRange2;
    export using ImGui::DragScalar;
    export using ImGui::DragScalarN;
    export using ImGui::SliderFloat;
    export using ImGui::SliderFloat2;
    export using ImGui::SliderFloat3;
    export using ImGui::SliderFloat4;
    export using ImGui::SliderAngle;
    export using ImGui::SliderInt;
    export using ImGui::SliderInt2;
    export using ImGui::SliderInt3;
    export using ImGui::SliderInt4;
    export using ImGui::SliderScalar;
    export using ImGui::SliderScalarN;
    export using ImGui::VSliderFloat;
    export using ImGui::VSliderInt;
    export using ImGui::VSliderScalar;
    export using ImGui::InputText;
    export using ImGui::InputTextMultiline;
    export using ImGui::InputTextWithHint;
    export using ImGui::InputFloat;
    export using ImGui::InputFloat2;
    export using ImGui::InputFloat3;
    export using ImGui::InputFloat4;
    export using ImGui::InputInt;
    export using ImGui::InputInt2;
    export using ImGui::InputInt3;
    export using ImGui::InputInt4;
    export using ImGui::InputDouble;
    export using ImGui::InputScalar;
    export using ImGui::InputScalarN;
    export using ImGui::ColorEdit3;
    export using ImGui::ColorEdit4;
    export using ImGui::ColorPicker3;
    export using ImGui::ColorPicker4;
    export using ImGui::ColorButton;
    export using ImGui::SetColorEditOptions;
    export using ImGui::TreeNode;
    export using ImGui::TreeNode;
    export using ImGui::TreeNode;
    export using ImGui::TreeNodeV;
    export using ImGui::TreeNodeV;
    export using ImGui::TreeNodeEx;
    export using ImGui::TreeNodeEx;
    export using ImGui::TreeNodeEx;
    export using ImGui::TreeNodeExV;
    export using ImGui::TreeNodeExV;
    export using ImGui::TreePush;
    export using ImGui::TreePush;
    export using ImGui::TreePop;
    export using ImGui::GetTreeNodeToLabelSpacing;
    export using ImGui::CollapsingHeader;
    export using ImGui::CollapsingHeader;
    export using ImGui::SetNextItemOpen;
    export using ImGui::SetNextItemStorageID;
    export using ImGui::Selectable;
    export using ImGui::Selectable;
    export using ImGui::BeginMultiSelect;
    export using ImGui::EndMultiSelect;
    export using ImGui::SetNextItemSelectionUserData;
    export using ImGui::IsItemToggledSelection;
    export using ImGui::BeginListBox;
    export using ImGui::EndListBox;
    export using ImGui::ListBox;
    export using ImGui::ListBox;
    export using ImGui::PlotLines;
    export using ImGui::PlotLines;
    export using ImGui::PlotHistogram;
    export using ImGui::PlotHistogram;
    export using ImGui::Value;
    export using ImGui::Value;
    export using ImGui::Value;
    export using ImGui::Value;
    export using ImGui::BeginMenuBar;
    export using ImGui::EndMenuBar;
    export using ImGui::BeginMainMenuBar;
    export using ImGui::EndMainMenuBar;
    export using ImGui::BeginMenu;
    export using ImGui::EndMenu;
    export using ImGui::MenuItem;
    export using ImGui::MenuItem;
    export using ImGui::BeginTooltip;
    export using ImGui::EndTooltip;
    export using ImGui::SetTooltip;
    export using ImGui::SetTooltipV;
    export using ImGui::BeginItemTooltip;
    export using ImGui::SetItemTooltip;
    export using ImGui::SetItemTooltipV;
    export using ImGui::BeginPopup;
    export using ImGui::BeginPopupModal;
    export using ImGui::EndPopup;
    export using ImGui::OpenPopup;
    export using ImGui::OpenPopup;
    export using ImGui::OpenPopupOnItemClick;
    export using ImGui::CloseCurrentPopup;
    export using ImGui::BeginPopupContextItem;
    export using ImGui::BeginPopupContextWindow;
    export using ImGui::BeginPopupContextVoid;
    export using ImGui::IsPopupOpen;
    export using ImGui::BeginTable;
    export using ImGui::EndTable;
    export using ImGui::TableNextRow;
    export using ImGui::TableNextColumn;
    export using ImGui::TableSetColumnIndex;
    export using ImGui::TableSetupColumn;
    export using ImGui::TableSetupScrollFreeze;
    export using ImGui::TableHeader;
    export using ImGui::TableHeadersRow;
    export using ImGui::TableAngledHeadersRow;
    export using ImGui::TableGetSortSpecs;
    export using ImGui::TableGetColumnCount;
    export using ImGui::TableGetColumnIndex;
    export using ImGui::TableGetRowIndex;
    export using ImGui::TableGetColumnName;
    export using ImGui::TableGetColumnFlags;
    export using ImGui::TableSetColumnEnabled;
    export using ImGui::TableGetHoveredColumn;
    export using ImGui::TableSetBgColor;
    export using ImGui::Columns;
    export using ImGui::NextColumn;
    export using ImGui::GetColumnIndex;
    export using ImGui::GetColumnWidth;
    export using ImGui::SetColumnWidth;
    export using ImGui::GetColumnOffset;
    export using ImGui::SetColumnOffset;
    export using ImGui::GetColumnsCount;
    export using ImGui::BeginTabBar;
    export using ImGui::EndTabBar;
    export using ImGui::BeginTabItem;
    export using ImGui::EndTabItem;
    export using ImGui::TabItemButton;
    export using ImGui::SetTabItemClosed;
    export using ImGui::DockSpace;
    #ifdef IMGUI_HAS_DOCK
	export using ImGui::DockSpaceOverViewport;
	#endif
    export using ImGui::SetNextWindowDockID;
    export using ImGui::SetNextWindowClass;
    export using ImGui::GetWindowDockID;
    export using ImGui::IsWindowDocked;
    export using ImGui::LogToTTY;
    export using ImGui::LogToFile;
    export using ImGui::LogToClipboard;
    export using ImGui::LogFinish;
    export using ImGui::LogButtons;
    export using ImGui::LogText;
    export using ImGui::LogTextV;
    export using ImGui::BeginDragDropSource;
    export using ImGui::SetDragDropPayload;
    export using ImGui::EndDragDropSource;
    export using ImGui::BeginDragDropTarget;
    export using ImGui::EndDragDropTarget;
    export using ::ImGuiPayload;
    export using ImGui::BeginDisabled;
    export using ImGui::EndDisabled;
    export using ImGui::PushClipRect;
    export using ImGui::PopClipRect;
    export using ImGui::SetItemDefaultFocus;
    export using ImGui::SetKeyboardFocusHere;
    export using ImGui::SetNavCursorVisible;
    export using ImGui::SetNextItemAllowOverlap;
    export using ImGui::IsItemHovered;
    export using ImGui::IsItemActive;
    export using ImGui::IsItemFocused;
    export using ImGui::IsItemClicked;
    export using ImGui::IsItemVisible;
    export using ImGui::IsItemEdited;
    export using ImGui::IsItemActivated;
    export using ImGui::IsItemDeactivated;
    export using ImGui::IsItemDeactivatedAfterEdit;
    export using ImGui::IsItemToggledOpen;
    export using ImGui::IsAnyItemHovered;
    export using ImGui::IsAnyItemActive;
    export using ImGui::IsAnyItemFocused;
    export using ImGui::GetItemID;
    export using ImGui::GetItemRectMin;
    export using ImGui::GetItemRectMax;
    export using ImGui::GetItemRectSize;
    export using ImGui::GetMainViewport;
    export using ImGui::GetBackgroundDrawList;
    export using ImGui::GetForegroundDrawList;
    export using ImGui::IsRectVisible;
    export using ImGui::IsRectVisible;
    export using ImGui::GetTime;
    export using ImGui::GetFrameCount;
    export using ImGui::GetDrawListSharedData;
    export using ImGui::GetStyleColorName;
    export using ImGui::SetStateStorage;
    export using ImGui::GetStateStorage;
    export using ImGui::CalcTextSize;
    export using ImGui::ColorConvertU32ToFloat4;
    export using ImGui::ColorConvertFloat4ToU32;
    export using ImGui::ColorConvertRGBtoHSV;
    export using ImGui::ColorConvertHSVtoRGB;
    export using ImGui::IsKeyDown;
    export using ImGui::IsKeyPressed;
    export using ImGui::IsKeyReleased;
    export using ImGui::IsKeyChordPressed;
    export using ImGui::GetKeyPressedAmount;
    export using ImGui::GetKeyName;
    export using ImGui::SetNextFrameWantCaptureKeyboard;
    export using ImGui::Shortcut;
    export using ImGui::SetNextItemShortcut;
    export using ImGui::SetItemKeyOwner;
    export using ImGui::IsMouseDown;
    export using ImGui::IsMouseClicked;
    export using ImGui::IsMouseReleased;
    export using ImGui::IsMouseDoubleClicked;
    export using ImGui::GetMouseClickedCount;
    export using ImGui::IsMouseHoveringRect;
    export using ImGui::IsMousePosValid;
    export using ImGui::IsAnyMouseDown;
    export using ImGui::GetMousePos;
    export using ImGui::GetMousePosOnOpeningCurrentPopup;
    export using ImGui::IsMouseDragging;
    export using ImGui::GetMouseDragDelta;
    export using ImGui::ResetMouseDragDelta;
    export using ImGui::GetMouseCursor;
    export using ImGui::SetMouseCursor;
    export using ImGui::SetNextFrameWantCaptureMouse;
    export using ImGui::GetClipboardText;
    export using ImGui::SetClipboardText;
    export using ImGui::LoadIniSettingsFromDisk;
    export using ImGui::LoadIniSettingsFromMemory;
    export using ImGui::SaveIniSettingsToDisk;
    export using ImGui::SaveIniSettingsToMemory;
    export using ImGui::DebugTextEncoding;
    export using ImGui::DebugFlashStyleColor;
    export using ImGui::DebugStartItemPicker;
    export using ImGui::DebugCheckVersionAndDataLayout;
    export using ImGui::DebugLog;
    export using ImGui::DebugLogV;
    export using ImGui::SetAllocatorFunctions;
    export using ImGui::GetAllocatorFunctions;
    export using ImGui::MemAlloc;
    export using ImGui::MemFree;
    export using ImGui::UpdatePlatformWindows;
    export using ImGui::RenderPlatformWindowsDefault;
    export using ImGui::DestroyPlatformWindows;
    export using ImGui::FindViewportByID;
    export using ImGui::FindViewportByPlatformHandle;
}

export using ::ImGuiWindowFlags_;
export using ::ImGuiChildFlags_;
export using ::ImGuiItemFlags_;
export using ::ImGuiInputTextFlags_;
export using ::ImGuiTreeNodeFlags_;
export using ::ImGuiPopupFlags_;
export using ::ImGuiSelectableFlags_;
export using ::ImGuiComboFlags_;
export using ::ImGuiTabBarFlags_;
export using ::ImGuiTabItemFlags_;
export using ::ImGuiFocusedFlags_;
export using ::ImGuiHoveredFlags_;
export using ::ImGuiDockNodeFlags_;
export using ::ImGuiDragDropFlags_;
export using ::ImGuiDataType_;
export using ::ImGuiDir;
export using ::ImGuiSortDirection;
export using ::ImGuiKey;
export using ::ImGuiInputFlags_;
export using ::ImGuiConfigFlags_;
export using ::ImGuiBackendFlags_;
export using ::ImGuiCol_;
export using ::ImGuiStyleVar_;
export using ::ImGuiButtonFlags_;
export using ::ImGuiColorEditFlags_;
export using ::ImGuiSliderFlags_;
export using ::ImGuiMouseButton_;
export using ::ImGuiMouseCursor_;
export using ::ImGuiMouseSource;
export using ::ImGuiCond_;
export using ::ImGuiTableFlags_;
export using ::ImGuiTableColumnFlags_;
export using ::ImGuiTableRowFlags_;
export using ::ImGuiTableBgTarget_;
export using ::ImGuiTableSortSpecs;
export using ::ImGuiTableColumnSortSpecs;
export using ::ImNewWrapper;
export using ::ImGuiStyle;
export using ::ImGuiKeyData;
export using ::ImGuiIO;
export using ::ImGuiInputTextCallbackData;
export using ::ImGuiSizeCallbackData;
export using ::ImGuiWindowClass;
export using ::ImGuiPayload;
export using ::ImGuiOnceUponAFrame;
export using ::ImGuiTextFilter;
export using ::ImGuiTextBuffer;
export using ::ImGuiStoragePair;
export using ::ImGuiStorage;
export using ::ImGuiListClipper;
// export using ::operator*;
// export using ::operator/;
// export using ::operator+;
// export using ::operator-;
// export using ::operator*;
// export using ::operator/;
// export using ::operator-;
// export using ::operator*=;
// export using ::operator/=;
// export using ::operator+=;
// export using ::operator-=;
// export using ::operator*=;
// export using ::operator/=;
// export using ::operator==;
// export using ::operator!=;
// export using ::operator+;
// export using ::operator-;
// export using ::operator*;
// export using ::operator==;
// export using ::operator!=;
export using ::ImColor;
export using ::ImGuiMultiSelectFlags_;
export using ::ImGuiMultiSelectIO;
export using ::ImGuiSelectionRequestType;
export using ::ImGuiSelectionRequest;
export using ::ImGuiSelectionBasicStorage;
export using ::ImGuiSelectionExternalStorage;
export using ::ImDrawCmd;
export using ::ImDrawVert;
export using ::ImDrawCmdHeader;
export using ::ImDrawChannel;
export using ::ImDrawListSplitter;
export using ::ImDrawFlags_;
export using ::ImDrawListFlags_;
export using ::ImDrawList;
export using ::ImDrawData;
export using ::ImFontConfig;
export using ::ImFontGlyph;
export using ::ImFontGlyphRangesBuilder;
export using ::ImFontAtlasCustomRect;
export using ::ImFontAtlasFlags_;
export using ::ImFontAtlas;
export using ::ImFont;
export using ::ImGuiViewportFlags_;
export using ::ImGuiViewport;
export using ::ImGuiPlatformIO;
export using ::ImGuiPlatformMonitor;
export using ::ImGuiPlatformImeData;
namespace ImGui
{
	// export using ImGui::PushButtonRepeat;
	// export using ImGui::PopButtonRepeat;
	// export using ImGui::PushTabStop;
	// export using ImGui::PopTabStop;
	export using ImGui::GetContentRegionMax;
	export using ImGui::GetWindowContentRegionMin;
	export using ImGui::GetWindowContentRegionMax;
	// export using ImGui::BeginChildFrame;
	// export using ImGui::EndChildFrame;
	// export using ImGui::ShowStackToolWindow;
	export using ImGui::Combo;
	export using ImGui::ListBox;
	export using ImGui::SetItemAllowOverlap;
	// export using ImGui::PushAllowKeyboardFocus;
	// export using ImGui::PopAllowKeyboardFocus;
}

// Internal
export using ::ImGuiDataVarInfo;
export using ::ImGuiStyleVar;
namespace ImGui
{
	export using ImGui::GetStyleVarInfo;
	export using ImGui::GetStyleVarInfo;
}

static inline constexpr auto IM_NewLine = IM_NEWLINE;

#endif
// NOLINTEND(misc-unused-using-decls)

_IMGUI_EXPORT 
inline void IMGUI_CheckVersion() {
	IMGUI_CHECKVERSION();
}