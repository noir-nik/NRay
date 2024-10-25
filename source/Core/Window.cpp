#include "Pch.hpp"

#include "Window.hpp"
// #include <imgui/imgui.h>

#define WINDOW_ALIVE_GUARD if (!alive) {LOG_WARN("ALIVE_GUARD {}:{}", __FILE__, __LINE__) return;}

namespace  {
struct Context
{
	// static inline GLFWmonitor** monitors           = nullptr;
	// static inline int           monitorCount       = 0;
	std::vector<std::string> pathsDrop;
	std::vector<Window>      windows;

	~Context() {
		WindowManager::Finish();
	}
};

static Context ctx;

}; // namespace WindowManager

namespace _WindowCallbacks {

void WindowPosCallback(GLFWwindow* window, int x, int y) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetPos(x, y);

	if (pWindow->windowPosCallback)
		pWindow->windowPosCallback(pWindow, x, y);
}

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetSize(width, height);

	if (pWindow->windowSizeCallback)
		pWindow->windowSizeCallback(pWindow, width, height);
}

void WindowCloseCallback(GLFWwindow* window) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	printf("Close callback triggered\n");

	if (pWindow->windowCloseCallback)
		pWindow->windowCloseCallback(pWindow);
}

void WindowRefreshCallback(GLFWwindow* window) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	// pWindow->SetDrawNeeded(true);

	if (pWindow->windowRefreshCallback)
		pWindow->windowRefreshCallback(pWindow);
}

void WindowFocusCallback(GLFWwindow* window, int focused) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);

	if (pWindow->windowFocusCallback)
		pWindow->windowFocusCallback(pWindow, focused);
}

void WindowIconifyCallback(GLFWwindow* window, int iconified) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	// LOG_INFO("Window {} iconified {}", pWindow->GetName(), iconified);

	if (pWindow->windowIconifyCallback)
		pWindow->windowIconifyCallback(pWindow, iconified);
}

void WindowMaximizeCallback(GLFWwindow* window, int maximize) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetMaximized(maximize);

	if (pWindow->windowMaximizeCallback)
		pWindow->windowMaximizeCallback(pWindow, maximize);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetSwapchainDirty(true);
	pWindow->SetDrawNeeded(true);
	// pWindow->SetFramebufferResized();
	// DEBUG_TRACE("Window {} framebuffer resized to {}x{}", pWindow->GetName(), width, height);
	
	if (pWindow->framebufferSizeCallback) 
		pWindow->framebufferSizeCallback(pWindow, width, height);
}

void WindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);

	if (pWindow->windowContentScaleCallback)
		pWindow->windowContentScaleCallback(pWindow, xscale, yscale);
}



} // namespace WindowCallbacks

namespace _InputCallbacks {

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		pWindow->SetResizable(!pWindow->GetResizable());
	}
	
	if (pWindow->mouseButtonCallback)
		pWindow->mouseButtonCallback(pWindow, button, action, mods);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);

	if (pWindow->cursorPosCallback)
		pWindow->cursorPosCallback(pWindow, xpos, ypos);
}

void CursorEnterCallback(GLFWwindow* window, int entered) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);

	if (pWindow->cursorEnterCallback)
		pWindow->cursorEnterCallback(pWindow, entered);
}

void ScrollCallback(GLFWwindow* window, double x, double y) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->GetScroll() += y;
	pWindow->GetDeltaScroll() += y;

	if (pWindow->scrollCallback)
		pWindow->scrollCallback(pWindow, x, y);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	if (pWindow->keyCallback)
		pWindow->keyCallback(pWindow, key, scancode, action, mods);
}

void CharCallback(GLFWwindow* window, unsigned int codepoint) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	if (pWindow->charCallback)
		pWindow->charCallback(pWindow, codepoint);
}

void CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	if (pWindow->charModsCallback)
		pWindow->charModsCallback(pWindow, codepoint, mods);
}


void DropCallback (GLFWwindow *window, int path_count, const char *paths[]) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	for (int i = 0; i < path_count; i++) {
		ctx.pathsDrop.push_back(paths[i]);
	}
	for (int i = 0; i < path_count; i++) {
		printf("%s\n", paths[i]);
	}

	if (pWindow->dropCallback)
		pWindow->dropCallback(pWindow, path_count, paths);
}
}


static void errorCallback(int error, const char* description)
{
	LOG_ERROR("[GLFW ERROR] ({}) {}", error, description);
}




void WindowManager::Init(){
	glfwSetErrorCallback(errorCallback);
	// initializing glfw
	auto res = glfwInit();

	if (!res) {
		LOG_CRITICAL("Failed to initialize GLFW");
		return;
	}

	// glfw uses OpenGL context by default
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	// // ctx.monitors = glfwGetMonitors(&ctx.monitorCount);
	// glfwGetVideoModes(ctx.monitors[monitorIndex], &videoModeIndex);
	// videoModeIndex -= 1;
	is_initialized = true;
}

// Call after all graphics contexts are destroyed
void WindowManager::Finish() {
	// Do not Log
	if (!is_initialized) return;
	printf("WindowManager::Finish()\n");
	glfwTerminate();
	is_initialized = false;
}

// Window factory
Window* WindowManager::NewWindow(int width, int height, const char* name) {
	if (!is_initialized) {
		Init();
	}
	Window* window = new Window(width, height, name);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	GLFWwindow* glfwWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
	// LOG_INFO("Window::Create({}x{}):{}", width, height, name);
	window->window = glfwWindow;
	window->GetPos();

	glfwSetWindowUserPointer(glfwWindow, window);

	glfwSetWindowPosCallback         (glfwWindow, _WindowCallbacks::WindowPosCallback         );
	glfwSetWindowSizeCallback        (glfwWindow, _WindowCallbacks::WindowSizeCallback        );
	glfwSetWindowCloseCallback       (glfwWindow, _WindowCallbacks::WindowCloseCallback       );
	glfwSetWindowRefreshCallback     (glfwWindow, _WindowCallbacks::WindowRefreshCallback     );
	glfwSetWindowFocusCallback       (glfwWindow, _WindowCallbacks::WindowFocusCallback       );
	glfwSetWindowIconifyCallback     (glfwWindow, _WindowCallbacks::WindowIconifyCallback     );
	glfwSetWindowMaximizeCallback    (glfwWindow, _WindowCallbacks::WindowMaximizeCallback    );
	glfwSetFramebufferSizeCallback   (glfwWindow, _WindowCallbacks::FramebufferSizeCallback   );
	glfwSetWindowContentScaleCallback(glfwWindow, _WindowCallbacks::WindowContentScaleCallback);

	glfwSetMouseButtonCallback       (glfwWindow, _InputCallbacks::MouseButtonCallback);
	glfwSetCursorPosCallback         (glfwWindow, _InputCallbacks::CursorPosCallback  );
	glfwSetScrollCallback            (glfwWindow, _InputCallbacks::ScrollCallback     );
	glfwSetKeyCallback               (glfwWindow, _InputCallbacks::KeyCallback        );
	glfwSetCharCallback              (glfwWindow, _InputCallbacks::CharCallback       );
	glfwSetCharModsCallback          (glfwWindow, _InputCallbacks::CharModsCallback   );
	glfwSetDropCallback              (glfwWindow, _InputCallbacks::DropCallback       );

	

	// window->dirty = false;
	window->ApplyChanges();
	// windowCount++;
	return window;
}

void Window::Destroy() {
	glfwGetWindowPos(window, &posX, &posY);
	glfwDestroyWindow(window);
	alive = false;
}

void Window::ApplyChanges() {
	WINDOW_ALIVE_GUARD
	// Destroy if should close
	if (GetShouldClose()) {
		Destroy();
		return;
	}
	
	// Change mode
	if (newMode != mode) {
		mode = newMode;
		swapchainDirty = true;
		drawNeeded = true;
		int monitorCount;
		auto monitors = glfwGetMonitors(&monitorCount);
		ASSERT(monitorIndex < monitorCount, "Invalid monitorIndex inside Window creation!", monitorIndex);
		auto monitor = monitors[monitorIndex];
		auto monitorMode = glfwGetVideoMode(monitor);

		int modesCount;
		const GLFWvidmode* videoModes = glfwGetVideoModes(monitors[monitorIndex], &modesCount);
		if (videoModeIndex >= modesCount) {
			videoModeIndex = modesCount - 1;
		}
		switch (newMode) {
		case WindowMode::Windowed:
			// posY = std::max(posY, 31);
			glfwSetWindowMonitor(window, nullptr, windowedSize.x, windowedSize.y, windowedSize.z, windowedSize.w, GLFW_DONT_CARE);
			if (maximized) {
				glfwMaximizeWindow(window);
			}
			// glfwSetWindowAttrib(window, GLFW_MAXIMIZED, maximized);
			glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable);
			glfwSetWindowAttrib(window, GLFW_DECORATED, decorated);
			break;
		case WindowMode::WindowedFullScreen:
			glfwWindowHint(GLFW_RED_BITS, monitorMode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, monitorMode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, monitorMode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, monitorMode->refreshRate);
			glfwSetWindowMonitor(window, monitor, 0, 0, monitorMode->width, monitorMode->height, monitorMode->refreshRate);
			break;
		case WindowMode::FullScreen:
			GLFWvidmode videoMode = videoModes[videoModeIndex];
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode.width, videoMode.height, videoMode.refreshRate);
			break;
		}
	}

	// framebufferResized = false;
}



void Window::Update() {
	WINDOW_ALIVE_GUARD
	// for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST + 1; i++) {
	// 	lastKeyState[i] = glfwGetKey(window, i);
	// }
	deltaScroll = 0;
	auto newTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
	deltaTime /= 1000.0f;
	lastTime = newTime;
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	deltaMousePos = mousePos - Lmath::vec2(x, y);
	mousePos = Lmath::vec2(x, y);
}

std::string VideoModeText(GLFWvidmode mode) {
	return std::to_string(mode.width) + "x" + std::to_string(mode.height) + " " + std::to_string(mode.refreshRate) + " Hz";
}

// void Window::OnImgui(){}

void Window::UpdateFramebufferSize() {
	WINDOW_ALIVE_GUARD
	// framebufferResized = false;
	glfwGetFramebufferSize(window, &width, &height);
}

bool Window::IsKeyPressed(uint16_t keyCode) {
	return lastKeyState[keyCode] && !glfwGetKey(window, keyCode);
}