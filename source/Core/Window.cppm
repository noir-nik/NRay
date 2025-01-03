#ifdef USE_MODULES
export module Window;
#define _WINDOW_EXPORT export
import lmath;
import Types;
import glfw;
import UI;
import Structs;
import std;
#else
#pragma once
#define _WINDOW_EXPORT
#include "lmath_types.hpp"
// #include "Runtime.cppm"
#include "Types.cppm"
#include "glfw.cppm"
#include "UI.cppm"
#include "Structs.cppm"

#include <chrono>
#include <vector>
#include <string>
#endif

_WINDOW_EXPORT
class WindowManager;
enum class WindowMode {
	Windowed,
	WindowedFullScreen,
	FullScreen,
};


/* =================================== Window Callbacks ================================= */
namespace _WindowCallbacks {
void WindowPosCallback          (GLFWwindow *window, int xpos, int ypos);
void WindowSizeCallback         (GLFWwindow *window, int width, int height);
void WindowCloseCallback        (GLFWwindow *window);
void WindowRefreshCallback      (GLFWwindow *window);
void WindowFocusCallback        (GLFWwindow *window, int focused);
void WindowIconifyCallback      (GLFWwindow *window, int iconified);
void WindowMaximizeCallback     (GLFWwindow *window, int maximized);
void FramebufferSizeCallback    (GLFWwindow *window, int width, int height);
void WindowContentScaleCallback (GLFWwindow *window, float xscale, float yscale);
}

/* =================================== Input Callbacks ================================= */
namespace _InputCallbacks {
void MouseButtonCallback (GLFWwindow *window, int button, int action, int mods);
void CursorPosCallback   (GLFWwindow *window, double xpos, double ypos);
void CursorEnterCallback (GLFWwindow *window, int entered);
void ScrollCallback      (GLFWwindow *window, double xoffset, double yoffset);
void KeyCallback         (GLFWwindow *window, int key, int scancode, int action, int mods);
void CharCallback        (GLFWwindow *window, unsigned int codepoint);
void CharModsCallback    (GLFWwindow *window, unsigned int codepoint, int mods);
void DropCallback        (GLFWwindow *window, int path_count, char const *paths[]);
}

_WINDOW_EXPORT
struct Mouse {
	float       scroll      = .0f;
	float       deltaScroll = .0f;
	lmath::vec2 pos         = lmath::vec2(.0f, .0f);
	lmath::vec2 deltaPos    = lmath::vec2(.0f, .0f);

	u8          buttons[8]  = { GLFW::Release, GLFW::Release, GLFW::Release, GLFW::Release, GLFW::Release, GLFW::Release, GLFW::Release, GLFW::Release };
	GLFW::Mod   mods        = static_cast<GLFW::Mod>(0);

	inline float GetScroll() const { return scroll; }
	inline float GetDeltaScroll() const { return deltaScroll; }
	inline lmath::vec2 GetPos() const { return pos; }
	inline lmath::vec2 GetDeltaPos() const { return deltaPos; }

	inline void ResetDeltaScroll() { deltaScroll = .0f; }

	inline int GetButton(GLFWwindow* window, GLFW::MouseButton button) { return glfwGetMouseButton(window, static_cast<int>(button)); }

private: 
	friend void _InputCallbacks::MouseButtonCallback (GLFWwindow *window, int button, int action, int mods);
	friend void _InputCallbacks::CursorPosCallback   (GLFWwindow *window, double xpos, double ypos);
	friend void _InputCallbacks::CursorEnterCallback (GLFWwindow *window, int entered);
	friend void _InputCallbacks::ScrollCallback      (GLFWwindow *window, double xoffset, double yoffset);
	friend void _InputCallbacks::KeyCallback         (GLFWwindow *window, int key, int scancode, int action, int mods);
	friend void _InputCallbacks::CharCallback        (GLFWwindow *window, unsigned int codepoint);
	friend void _InputCallbacks::CharModsCallback    (GLFWwindow *window, unsigned int codepoint, int mods);
	friend void _InputCallbacks::DropCallback        (GLFWwindow *window, int path_count, char const *paths[]);
};
_WINDOW_EXPORT
extern Mouse mouse;

_WINDOW_EXPORT
struct WindowCreateInfo {
	lmath::ivec2 size = { 1280, 960 };
	lmath::ivec2 pos = { 30, 30 };
	char const* name = "Window";
	void* imGuiStyle = nullptr;
};

_WINDOW_EXPORT
class Window : Structs::NoCopyNoMove {
	friend class WindowManager;
	GLFWwindow*   window             = nullptr;
	void*         surface            = nullptr;
	std::string   name               = "Window";
	lmath::int2   pos                = { 0, 30 };
	lmath::int2   size               = { 640, 480 };
	int           monitorIndex       = 0;
	int           videoModeIndex     = 0;
	lmath::vec2   scale              = { 1.0f, 1.0f };
	lmath::ivec4  windowedSize       = { 30, 30, 640, 480 };
	lmath::ivec4  sizeLimits         = { 30, 30, 1920, 1080 };
	static inline bool          framebufferResized = false;
	int frameCount = 0;

	std::chrono::high_resolution_clock::time_point lastTime;
	float deltaTime = .0f;

	EntityType entityHandle;

	std::vector<std::string> pathsDrop;

	// char lastKeyState[GLFW::KEY_LAST + 1];
	WindowMode mode = WindowMode::Windowed;
	WindowMode newMode = WindowMode::Windowed;
	const GLFWvidmode* vidMode = nullptr;

	int framesToDraw = 1;
	// bool swapchainDirty = false;

	bool resizable = true;
	bool decorated = true;
	bool maximized = false;
	
	bool alive = true;
	bool swapChainAlive = false;

	bool focused = true;

	UI::Context UIContext;

	
	
	/* ================================ User Callbacks ===================================================== */

	void (*windowPosCallback)          (Window *window, int xpos, int ypos)                          = nullptr;
	void (*windowSizeCallback)         (Window *window, int width, int height)                       = nullptr;
	void (*windowCloseCallback)        (Window *window)                                              = nullptr;
	void (*windowRefreshCallback)      (Window *window)                                              = nullptr;
	void (*windowFocusCallback)        (Window *window, int focused)                                 = nullptr;
	void (*windowIconifyCallback)      (Window *window, int iconified)                               = nullptr;
	void (*windowMaximizeCallback)     (Window *window, int maximized)                               = nullptr;
	void (*framebufferSizeCallback)    (Window *window, int width, int height)                       = nullptr;
	void (*windowContentScaleCallback) (Window *window, float xscale, float yscale)                  = nullptr;

	void (*mouseButtonCallback)        (Window *window, int button, int action, int mods)            = nullptr;
	void (*cursorPosCallback)          (Window *window, double xpos, double ypos)                    = nullptr;
	void (*cursorEnterCallback)        (Window *window, int entered)                                 = nullptr;
	void (*scrollCallback)             (Window *window, double xoffset, double yoffset)              = nullptr;
	void (*keyCallback)                (Window *window, int key, int scancode, int action, int mods) = nullptr;
	void (*charCallback)               (Window *window, unsigned int codepoint)                      = nullptr;
	void (*charModsCallback)           (Window *window, unsigned int codepoint, int mods)            = nullptr;
	void (*dropCallback)               (Window *window, int path_count, char const *paths[])         = nullptr;
		
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;
	Window(const Window&) = delete;
	Window(Window&&) = delete;

public:
	// void* uiDrawData = nullptr;

	Window(WindowCreateInfo const& info);
	inline ~Window(){ if(alive) Destroy(); }
	void ApplyChanges();
	void UpdateFramebufferSize();
	void Update();
	bool IsKeyPressed(u16 keyCode);
	void Destroy();

	[[nodiscard]] inline operator GLFWwindow*() const      { return window; }
	inline GLFWwindow* GetGLFWwindow()                     { return window; }
	inline void*       GetSurface()                        { return surface; }
	inline void        SetSurface(void* value)             { surface = value; }
	inline EntityType  GetEntityHandle()                   { return entityHandle; }
	inline void        SetEntityHandle(EntityType handle)  { entityHandle = handle; }
	inline void        WaitEvents()                        { glfwWaitEvents(); }
	inline int         GetWidth()                          { return size.x; }
	inline int         GetHeight()                         { return size.y; }
	inline auto        GetSize()                           { return size; }
	inline void        StoreWindowSize()                   { windowedSize = {pos.x, pos.y, size.x, size.y}; }
	inline auto        GetFrameCount()                     { return frameCount; }
	inline void        SetFrameCount(int value)            { frameCount = value; }

	inline int         GetMonitorIndex()                   { return monitorIndex; }
	inline int         GetMonitorWidth()                   { vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor()); return vidMode->width; }
	inline int         GetMonitorHeight()                  { vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor()); return vidMode->height; }

	inline float       GetDeltaTime()                      { return deltaTime; }

	inline bool        GetShouldClose()                    { return glfwWindowShouldClose(window); }
	inline void        SetShouldClose(bool value)          { glfwSetWindowShouldClose(window, value); }

	// inline float&      GetScroll()                         { return scroll;                                 }
	// inline float&      GetDeltaScroll()                    { return deltaScroll;                            }

	// inline lmath::vec2 GetDeltaMouse()                     { return deltaMousePos;                          }

	inline bool        IsKeyDown(u16 keyCode)              { return glfwGetKey(window, keyCode);            }
	inline bool        IsMouseDown(u16 buttonCode)         { return glfwGetMouseButton(window, buttonCode); }


	inline void        CmdResizeTo(int width, int height)  { glfwSetWindowSize(window, width, height); framesToDraw += 1; }
	inline void        SetSize(lmath::int2 value)          { size = value; }
	inline void        SetSize(int width, int height)      { size = {width, height}; }
	
	inline auto        GetPos()                            { glfwGetWindowPos(window, &pos.x, &pos.y); return lmath::int2(pos.x, pos.y); }
	inline void        SetPos(int x, int y)                { pos.x = x; pos.y = y; }

	inline auto        GetMode()                           { return mode; }
	inline void        SetMode(WindowMode value)           { newMode = value; }

	inline bool        GetFramebufferResized()             { return framebufferResized; }
	inline void        SetFramebufferResized(bool value)   { framebufferResized = value; }

	inline char const* GetName()                           { return name.c_str(); }
	inline void        SetName(char const* name)           { glfwSetWindowTitle(window, name);}

	inline bool        GetMaximized()                      { return maximized; }
	inline void        SetMaximized(bool value)            { maximized = value;}

	inline bool        GetDrawNeeded()                     { return framesToDraw > 0; }
	inline int         GetFramesToDraw()                   { return framesToDraw; }
	inline void        AddFramesToDraw(int value)           { framesToDraw += value; }
	inline void        SetFramesToDraw(int value)           { framesToDraw = value; }

	inline void        SetUIContextCurrent()               { UIContext.SetCurrent(); }

	inline bool        GetAlive()                          { return alive; }
	inline void        SetAlive(bool value)                { alive = value; }

	inline bool        GetResizable()                      { return resizable; }
	inline void        SetResizable(bool value)            { resizable = value; glfwSetWindowAttrib(window, GLFW::Resizable, value); }

	inline bool        GetDecorated()                      { return decorated; }
	inline void        SetDecorated(bool value)            { decorated = value; glfwSetWindowAttrib(window, GLFW::Decorated, value); }

	inline void        GetFocused()                        { glfwGetWindowAttrib(window, GLFW::Focused); }

	inline bool        GetIconified()                      { glfwGetWindowSize(window, &size.x, &size.y); return size.x == 0 || size.y == 0; }

	inline void        SetMinSize(int w, int h)            { sizeLimits.x = w; sizeLimits.y = h; glfwSetWindowSizeLimits(window, w, h, GLFW::DontCare, GLFW::DontCare); }
	inline void        SetMaxSize(int w, int h)            { sizeLimits.z = w; sizeLimits.w = h; glfwSetWindowSizeLimits(window, GLFW::DontCare, GLFW::DontCare, w, h); }
	inline auto        GetSizeLimits()                     { return sizeLimits; }

	inline auto        GetContentScale()                   { glfwGetWindowContentScale(window, &scale.x, &scale.y); return scale; }

	inline void        CmdShow()                           { glfwShowWindow(window); }
	inline void        CmdHide()                           { glfwHideWindow(window); }
	inline void        CmdClose()                          { glfwSetWindowShouldClose(window, true); }
	inline void        CmdFocus()                          { glfwFocusWindow(window); }
	inline void        CmdIconify()                        { glfwIconifyWindow(window); }
	inline void        CmdRestore()                        { glfwRestoreWindow(window); }
	inline void        CmdMaximize()                       { glfwMaximizeWindow(window); }
	inline void        CmdSetPos(int x, int y)             { glfwSetWindowPos(window, x, y); }
	inline void        CmdSetPos(lmath::int2 pos)          { glfwSetWindowPos(window, pos.x, pos.y); }



	/* =============================================================== Add User Callbacks =========================================================================== */

	inline void AddWindowPosCallback           (void(*callback)(Window *window, int xpos, int ypos))                          { windowPosCallback = callback;          }
	inline void AddWindowSizeCallback          (void(*callback)(Window *window, int width, int height))                       { windowSizeCallback = callback;         }
	inline void AddWindowCloseCallback         (void(*callback)(Window *window))                                              { windowCloseCallback = callback;        }
	inline void AddWindowRefreshCallback       (void(*callback)(Window *window))                                              { windowRefreshCallback = callback;      }
	inline void AddWindowFocusCallback         (void(*callback)(Window *window, int focused))                                 { windowFocusCallback = callback;        }
	inline void AddWindowIconifyCallback       (void(*callback)(Window *window, int iconified))                               { windowIconifyCallback = callback;      }
	inline void AddWindowMaximizeCallback      (void(*callback)(Window *window, int maximized))                               { windowMaximizeCallback = callback;     }
	inline void AddFramebufferSizeCallback     (void(*callback)(Window *window, int width, int height))                       { framebufferSizeCallback = callback;    }
	inline void AddWindowContentScaleCallback  (void(*callback)(Window *window, float xscale, float yscale))                  { windowContentScaleCallback = callback; }

	inline void AddMouseButtonCallback         (void(*callback)(Window *window, int button, int action, int mods))            { mouseButtonCallback = callback;        }
	inline void AddCursorPosCallback           (void(*callback)(Window *window, double xpos, double ypos))                    { cursorPosCallback = callback;          }
	inline void AddCursorEnterCallback         (void(*callback)(Window *window, int entered))                                 { cursorEnterCallback = callback;        }
	inline void AddScrollCallback              (void(*callback)(Window *window, double xoffset, double yoffset))              { scrollCallback = callback;             }
	inline void AddKeyCallback                 (void(*callback)(Window *window, int key, int scancode, int action, int mods)) { keyCallback = callback;                }
	inline void AddCharCallback                (void(*callback)(Window *window, unsigned int codepoint))                      { charCallback = callback;               }
	inline void AddCharModsCallback            (void(*callback)(Window *window, unsigned int codepoint, int mods))            { charModsCallback = callback;           }
	inline void AddDropCallback                (void(*callback)(Window *window, int path_count, char const *paths[]))         { dropCallback = callback;               }

private:
	friend void _WindowCallbacks::WindowPosCallback          (GLFWwindow *window, int xpos, int ypos);
	friend void _WindowCallbacks::WindowSizeCallback         (GLFWwindow *window, int width, int height);
	friend void _WindowCallbacks::WindowCloseCallback        (GLFWwindow *window);
	friend void _WindowCallbacks::WindowRefreshCallback      (GLFWwindow *window);
	friend void _WindowCallbacks::WindowFocusCallback        (GLFWwindow *window, int focused);
	friend void _WindowCallbacks::WindowIconifyCallback      (GLFWwindow *window, int iconified);
	friend void _WindowCallbacks::WindowMaximizeCallback     (GLFWwindow *window, int maximized);
	friend void _WindowCallbacks::FramebufferSizeCallback    (GLFWwindow *window, int width, int height);
	friend void _WindowCallbacks::WindowContentScaleCallback (GLFWwindow *window, float xscale, float yscale);

	friend void _InputCallbacks::MouseButtonCallback         (GLFWwindow *window, int button, int action, int mods);
	friend void _InputCallbacks::CursorPosCallback           (GLFWwindow *window, double xpos, double ypos);
	friend void _InputCallbacks::CursorEnterCallback         (GLFWwindow *window, int entered);
	friend void _InputCallbacks::ScrollCallback              (GLFWwindow *window, double xoffset, double yoffset);
	friend void _InputCallbacks::KeyCallback                 (GLFWwindow *window, int key, int scancode, int action, int mods);
	friend void _InputCallbacks::CharCallback                (GLFWwindow *window, unsigned int codepoint);
	friend void _InputCallbacks::CharModsCallback            (GLFWwindow *window, unsigned int codepoint, int mods);
	friend void _InputCallbacks::DropCallback                (GLFWwindow *window, int path_count, char const *paths[]);

	inline std::vector<std::string> GetAndClearPaths()     { auto paths = pathsDrop; pathsDrop.clear(); return paths; }
};

class WindowManager {
friend class Window;
public:
	WindowManager() = delete;
	static void Init();
	static inline void PollEvents(){ glfwPollEvents(); }
	static inline void WaitEvents(){ glfwWaitEvents(); }
	// void OnImgui();

	static void Finish();
private:
	static inline bool is_initialized = false;
};
