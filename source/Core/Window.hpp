#pragma once
#include <GLFW/glfw3.h>
#include <chrono>
#include <vector>
#include <string>
#include "Lmath.hpp"


class WindowManager;
enum class WindowMode {
	Windowed,
	WindowedFullScreen,
	FullScreen,
};

class Window {
	friend class WindowManager;
	GLFWwindow*   window             = nullptr;
	const char*   name               = "Engine";
	int           width              = 640;
	int           height             = 480;
	int           posX               = 0;
	int           posY               = 30;
	int           monitorIndex       = 0;
	int           videoModeIndex     = 0;
	// bool          framebufferResized = false;
	Lmath::ivec4  windowedSize        = { 30, 30, 640, 480 };

	std::chrono::high_resolution_clock::time_point lastTime;
	float deltaTime = .0f;

	std::vector<std::string> pathsDrop;

	float       scroll        = .0f;
	float       deltaScroll   = .0f;
	Lmath::vec2 mousePos      = Lmath::vec2(.0f, .0f);
	Lmath::vec2 deltaMousePos = Lmath::vec2(.0f, .0f);

	char lastKeyState[GLFW_KEY_LAST + 1];
	WindowMode mode = WindowMode::Windowed;
	WindowMode newMode = WindowMode::Windowed;
	const GLFWvidmode* vidMode = nullptr;

	// bool dirty     = true;
	bool drawNeeded = true;
	bool swapchainDirty = false;
	bool resizable = true;
	bool decorated = true;
	bool maximized = false;
	bool alive = true;
	// bool shouldClose = false;

public:
	Window(int width, int height, const char* name = "Engine"): width(width), height(height), name(name) {}
	Window& operator=(const Window&) = delete;
	Window(const Window&) = delete;
	virtual ~Window(){ if(alive) Destroy(); }
	void ApplyChanges();
	void UpdateFramebufferSize();
	void Update();
	bool IsKeyPressed(uint16_t keyCode);
	void Destroy();

	inline GLFWwindow* GetGLFWwindow()                     { return window;                                 }
	// inline bool        IsDirty()                           { return dirty;                                  }
	inline void        WaitEvents()                        { glfwWaitEvents();                              }
	inline int         GetWidth()                          { return width;                                  }
	inline int         GetHeight()                         { return height;                                 }
	inline auto        GetSize()                           { return Lmath::ivec2(width, height);            }
	inline void        StoreWindowSize()                   { windowedSize = {posX, posY, width, height};    }

	inline int         GetMonitorIndex()                   { return monitorIndex;                           }
	inline int         GetMonitorWidth()                   { vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor()); return vidMode->width; }
	inline int         GetMonitorHeight()                  { vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor()); return vidMode->height; }

	inline float       GetDeltaTime()                      { return deltaTime;                              }

	inline bool        GetShouldClose()                    { return glfwWindowShouldClose(window);          }
	inline void        SetShouldClose(bool value)          { glfwSetWindowShouldClose(window, value);       }

	inline float&      GetScroll()                         { return scroll;                                 }
	inline float&      GetDeltaScroll()                    { return deltaScroll;                            }

	inline Lmath::vec2 GetDeltaMouse()                     { return deltaMousePos;                          }

	// inline bool        GetFramebufferResized()             { return framebufferResized;                     }
	inline bool        IsKeyDown(uint16_t keyCode)         { return glfwGetKey(window, keyCode);            }
	inline bool        IsMouseDown(uint16_t buttonCode)    { return glfwGetMouseButton(window, buttonCode); }
	inline void        SetUserPointer(void *user_ptr)      { glfwSetWindowUserPointer(window, user_ptr);    }

	inline void        CmdResizeTo(int width, int height)  { glfwSetWindowSize(window, width, height); drawNeeded = true; }
	inline void        SetSize(int w, int h)               { width = w; height = h;}
	
	inline Lmath::int2 GetPos()                            { glfwGetWindowPos(window, &posX, &posY); return {posX, posY}; }
	inline void        SetPos(int x, int y)                { posX = x; posY = y; }

	inline WindowMode  GetMode()                           { return mode; }
	inline void        SetMode(WindowMode value)           { newMode = value; }

	// inline bool        GetFullscreen()                     { return mode == WindowMode::FullScreen; }


	// inline void        SetFramebufferResized()             { framebufferResized = true; }

	inline const char* GetName()                           { return name; }
	inline void        SetName(const char* name)           { glfwSetWindowTitle(window, name);               }

	inline bool        GetMaximized()                      { return maximized; }
	inline void        SetMaximized(bool value)            { maximized = value;}

	inline bool        GetDrawNeeded()                     { return drawNeeded; }
	inline void        SetDrawNeeded(bool value)           { drawNeeded = value; }
	inline bool        GetSwapchainDirty()                 { return swapchainDirty; }
	inline void        SetSwapchainDirty(bool value)       { swapchainDirty = value; }

	inline bool        GetAlive()                          { return alive; }
	inline void        SetAlive(bool value)                { alive = value; }

	inline bool        GetResizable()                      { return resizable; }
	inline void        SetResizable(bool value)            { resizable = value; glfwSetWindowAttrib(window, GLFW_RESIZABLE, value); }

	inline bool        GetDecorated()                      { return decorated; }
	inline void        SetDecorated(bool value)            { decorated = value; glfwSetWindowAttrib(window, GLFW_DECORATED, value); }

	inline bool        GetIconified()                      { glfwGetWindowSize(window, &width, &height); return width == 0 || height == 0; }

	inline void        SetMinSize(int w, int h)            { glfwSetWindowSizeLimits(window, w, h, GLFW_DONT_CARE, GLFW_DONT_CARE); }
	inline void        SetMaxSize(int w, int h)            { glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, w, h); }

	inline void        SetFramebufferSizeCallback(void(*callback)(GLFWwindow* window, int width, int height)) { glfwSetFramebufferSizeCallback(window, callback); }
	inline void        SetDropCallback(void(*callback)(GLFWwindow* window, int count, const char** paths)) { glfwSetDropCallback(window, callback); }
	inline void        SetKeyCallback(void(*callback)(GLFWwindow* window, int key, int scancode, int action, int mods)) { glfwSetKeyCallback(window, callback); }
	inline void        SetMouseButtonCallback(void(*callback)(GLFWwindow* window, int button, int action, int mods)) { glfwSetMouseButtonCallback(window, callback); }
	inline void        SetScrollCallback(void(*callback)(GLFWwindow* window, double xoffset, double yoffset)) { glfwSetScrollCallback(window, callback); }


	inline std::vector<std::string> GetAndClearPaths()     { auto paths = pathsDrop; pathsDrop.clear(); return paths; }
};

/* 
class MainWindow : public Window {
	std::set<Window*> children;
public:
	MainWindow(int width, int height, const char* name) : Window(width, height, name) {}

	~MainWindow() {
		for (auto child : children) {
			delete child;
		}
	}
};
 */

class WindowManager {
friend class Window;
public:
	WindowManager() = delete;
	static void Init();
	static Window* NewWindow(int width, int height, const char* name);
	static void PollEvents(){ glfwPollEvents(); }
	static void WaitEvents(){ glfwWaitEvents(); }
	// void OnImgui();

	static void Finish();
private:
	static inline bool is_initialized = false;
	// static inline int  windowCount = 0;
};