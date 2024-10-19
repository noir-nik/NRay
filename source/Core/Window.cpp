#include "Pch.hpp"

#include "Window.hpp"
// #include <imgui/imgui.h>

static void ScrollCallback(GLFWwindow* window, double x, double y);
static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
static void WindowMaximizeCallback(GLFWwindow* window, int maximized);
static void WindowChangePosCallback(GLFWwindow* window, int x, int y);
static void WindowDropCallback(GLFWwindow* window, int count, const char* paths[]);

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



// void ScrollCallback(GLFWwindow* window, double x, double y) {
// 	Window::scroll += y;
// 	Window::deltaScroll += y;
// }

// void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
// 	Window::width = width;
// 	Window::height = height;
// 	Window::framebufferResized = true;
// }

// void WindowMaximizeCallback(GLFWwindow* window, int maximize) {
// 	maximized = maximize;
// }

// void WindowChangePosCallback(GLFWwindow* window, int x, int y) {
// 	Window::posX = x;
// 	Window::posY = y;
// }

// void WindowDropCallback(GLFWwindow* window, int count, const char* paths[]) {
// 	for (int i = 0; i < count; i++) {
// 		pathsDrop.push_back(paths[i]);
// 	}
// 	for (int i = 0; i < count; i++) {
// 		printf("%s\n", paths[i]);
// 	}
// }


void errorCallback(int error, const char* description)
{
	LOG_ERROR("[GLFW ERROR] ({}) {}", error, description);
}

}; // namespace WindowManager


void WindowManager::Init(){
	glfwSetErrorCallback(errorCallback);
	// initializing glfw
	glfwInit();

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
	glfwSetWindowUserPointer(glfwWindow, window.get());
	glfwGetWindowPos(glfwWindow, &window->posX, &window->posY);
	// glfwSetWindowPos(window, posX, posY);
	// glfwSetFramebufferSizeCallback(glfwWindow, FramebufferSizeCallback);
	// glfwSetScrollCallback(glfwWindow, ScrollCallback);
	// glfwSetWindowMaximizeCallback(glfwWindow, WindowMaximizeCallback);
	// glfwSetWindowPosCallback(glfwWindow, WindowChangePosCallback);
	// glfwSetDropCallback(glfwWindow, WindowDropCallback);

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
	for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST + 1; i++) {
		lastKeyState[i] = glfwGetKey(window, i);
	}
	deltaScroll = 0;
	auto newTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
	deltaTime /= 1000.0f;
	lastTime = newTime;
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	deltaMousePos = mousePos - Lmath::vec2(x, y);
	mousePos = Lmath::vec2(x, y);
	glfwPollEvents();
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