#include "Pch.hpp"

#include "Window.hpp"
// #include <imgui/imgui.h>

#define WINDOW_ALIVE_GUARD if (window == nullptr) return;

namespace {
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



void ScrollCallback(GLFWwindow* window, double x, double y) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->GetScroll() += y;
	pWindow->GetDeltaScroll() += y;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetSize(width, height);
	pWindow->SetFramebufferResized(true);
	DEBUG_TRACE("Window {} framebuffer resized to {}x{}", pWindow->GetName(), width, height);
}

void WindowMaximizeCallback(GLFWwindow* window, int maximize) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetMaximized(maximize);
}

void WindowChangePosCallback(GLFWwindow* window, int x, int y) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetPos(x, y);
}

void WindowDropCallback(GLFWwindow* window, int count, const char* paths[]) {
	for (int i = 0; i < count; i++) {
		ctx.pathsDrop.push_back(paths[i]);
	}
	for (int i = 0; i < count; i++) {
		printf("%s\n", paths[i]);
	}
}

void WindowCloseCallback(GLFWwindow* window) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetShouldClose(true);
	printf("Close callback triggered\n");
}


void errorCallback(int error, const char* description)
{
	LOG_ERROR("[GLFW ERROR] ({}) {}", error, description);
}

}; // namespace WindowManager


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

std::shared_ptr<Window> WindowManager::NewWindow(int width, int height, const char* name) {
	if (!is_initialized) {
		Init();
	}
	auto window = std::make_shared<Window>(width, height, name);
	GLFWwindow* glfwWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
	window->window = glfwWindow;
	window->SetUserPointer(window.get());
	window->GetPos();
	
	// glfwSetFramebufferSizeCallback(glfwWindow, FramebufferSizeCallback);
	// glfwSetScrollCallback(glfwWindow, ScrollCallback);
	// glfwSetWindowMaximizeCallback(glfwWindow, WindowMaximizeCallback);
	glfwSetWindowPosCallback(glfwWindow, WindowChangePosCallback);
	// glfwSetDropCallback(glfwWindow, WindowDropCallback);
	glfwSetWindowCloseCallback(glfwWindow, WindowCloseCallback);

	window->dirty = false;
    window->ApplyChanges();
	// windowCount++;
	return window;
}

void Window::Destroy() {
	glfwGetWindowPos(window, &posX, &posY);
	glfwDestroyWindow(window);
	window = nullptr;
	LOG_INFO("Window::Destroy()");
	// WindowManager::windowCount--;
	// if (WindowManager::windowCount == 0) {
	// 	WindowManager::Finish();
	// }
}

void Window::ApplyChanges() {
	WINDOW_ALIVE_GUARD
	if (GetShouldClose()) {
		Destroy();
		return;
	}

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

	// creating window
	switch (mode) {
	case WindowMode::Windowed:
		// posY = std::max(posY, 31);
		// glfwSetWindowMonitor(window, nullptr, posX, posY, width, height, GLFW_DONT_CARE);
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

	framebufferResized = false;
	dirty = false;
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
	framebufferResized = false;
	glfwGetFramebufferSize(window, &width, &height);
}

bool Window::IsKeyPressed(uint16_t keyCode) {
	return lastKeyState[keyCode] && !glfwGetKey(window, keyCode);
}