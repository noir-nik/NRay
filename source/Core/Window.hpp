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
	bool          framebufferResized = false;

	std::chrono::high_resolution_clock::time_point lastTime;
	float deltaTime = .0f;

	std::vector<std::string> pathsDrop;

	float       scroll        = .0f;
	float       deltaScroll   = .0f;
	Lmath::vec2 mousePos      = Lmath::vec2(.0f, .0f);
	Lmath::vec2 deltaMousePos = Lmath::vec2(.0f, .0f);

	char lastKeyState[GLFW_KEY_LAST + 1];
	WindowMode mode = WindowMode::Windowed;
	bool dirty     = true;
	bool resizable = true;
	bool decorated = true;
	bool maximized = false;

public:
	Window(int width, int height, const char* name = "Engine"): width(width), height(height), name(name) {}
	~Window(){ if(window) Destroy(); }
	void ApplyChanges();
	void UpdateFramebufferSize();
	void Update();
	bool IsKeyPressed(uint16_t keyCode);
	void Destroy();

	inline GLFWwindow* GetGLFWwindow()                  { return window;                                 }
	inline bool        IsDirty()                        { return dirty;                                  }
	inline void        WaitEvents()                     { glfwWaitEvents();                              }
	inline uint32_t    GetWidth()                       { return width;                                  }
	inline uint32_t    GetHeight()                      { return height;                                 }
	inline float       GetDeltaTime()                   { return deltaTime;                              }
	inline bool        GetShouldClose()                 { return glfwWindowShouldClose(window);          }
	inline float       GetDeltaScroll()                 { return deltaScroll;                            }
	inline Lmath::vec2 GetDeltaMouse()                  { return deltaMousePos;                          }
	inline bool        GetFramebufferResized()          { return framebufferResized;                     }
	inline bool        IsKeyDown(uint16_t keyCode)      { return glfwGetKey(window, keyCode);            }
	inline bool        IsMouseDown(uint16_t buttonCode) { return glfwGetMouseButton(window, buttonCode); }
	inline void        SetUserPointer(void *user_ptr)   { glfwSetWindowUserPointer(window, user_ptr);    }

	inline void        SetMode(WindowMode newMode)      { mode = newMode; dirty = true; }
	inline std::vector<std::string> GetAndClearPaths()  { auto paths = pathsDrop; pathsDrop.clear(); return paths; }
};

class WindowManager {
friend class Window;
public:
	WindowManager() = delete;
	static void Init();
	static std::shared_ptr<Window> NewWindow(int width, int height, const char* name);
	// void OnImgui();

	static void Finish();
private:
	static inline bool is_initialized = false;
	// static inline int  windowCount = 0;
};