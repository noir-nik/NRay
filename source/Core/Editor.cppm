export module Editor;
import lmath;
import std;
import SceneGraph;
import Camera;
import Structs;

export
namespace Editor {
using namespace lmath;
using Engine::Camera;
using UiDrawData = void;
using UiStyle = void;

struct PanelBase {
  Camera camera{};
  ivec4 rect{};
};

struct Viewport : PanelBase {
};

struct Outliner : PanelBase {

};

struct Properties : PanelBase {
	
};

using Panel = std::variant<Viewport, Outliner, Properties>;

struct WindowData : Structs::NoCopyNoMove {
	bool main = false;
	std::vector<Panel> panels;
	std::span<Panel> tabPanels;
	
	std::vector<Viewport*> viewportsToRender;

	WindowData(bool main) : main(main) {}

};

struct Data {
	SceneGraph* sceneGraph;
	WindowData* windowData;
};

	void Setup(SceneGraph* sceneGraph);
	void SetupWindow(WindowData& windowData);
	void Finish();
    void BeginFrame();
	void Draw(Data const& data);
    UiDrawData* EndFrame();
	UiStyle* GetStyle();
        }; // namespace Editor

#undef _EDITOR_EXPORT