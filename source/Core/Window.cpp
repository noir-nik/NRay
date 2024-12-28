#ifdef USE_MODULES
module Window;
import Lmath;
import glfw;
import Log;
#else
#include "Window.cppm"
#include "Lmath.cppm"
#include "glfw.cppm"
#include "Log.cppm"
#endif
// #include <imgui/imgui.h>

#define WINDOW_ALIVE_GUARD if (!alive) {LOG_WARN("ALIVE_GUARD {}:{}", __FILE__, __LINE__); return;}
#define TRACE_WINDOW 0
#define TRACE_INPUT 0
#if TRACE_WINDOW
#define LOG_WINDOW(...) Logger::Get()->trace(__VA_ARGS__)
#else
#define LOG_WINDOW(...)
#endif

#if TRACE_INPUT
#define LOG_INPUT(...) Logger::Get()->trace(__VA_ARGS__)
#else
#define LOG_INPUT(...)
#endif

namespace  {
struct Context
{
	// static inline GLFWmonitor** monitors           = nullptr;
	// static inline int           monitorCount       = 0;
	std::vector<std::string> pathsDrop;
	// std::vector<Window>      windows;

	~Context() {
		WindowManager::Finish();
	}
};

static Context ctx;

}; // namespace WindowManager
Mouse mouse;

namespace _WindowCallbacks {

void WindowPosCallback(GLFWwindow* window, int x, int y) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} moved to {}x{}", pWindow->GetName(), x, y);
	pWindow->SetPos(x, y);

	if (pWindow->windowPosCallback)
		pWindow->windowPosCallback(pWindow, x, y);
}

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} resized to {}x{}", pWindow->GetName(), width, height);
	pWindow->SetSize(width, height);

	if (pWindow->windowSizeCallback)
		pWindow->windowSizeCallback(pWindow, width, height);
}

void WindowCloseCallback(GLFWwindow* window) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} closed", pWindow->GetName());

	if (pWindow->windowCloseCallback)
		pWindow->windowCloseCallback(pWindow);
}

void WindowRefreshCallback(GLFWwindow* window) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} refreshed", pWindow->GetName());

	if (pWindow->windowRefreshCallback)
		pWindow->windowRefreshCallback(pWindow);
}

void WindowFocusCallback(GLFWwindow* window, int focused) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} focused {}", pWindow->GetName(), focused);

	if (focused) pWindow->SetUIContextCurrent();

	if (pWindow->windowFocusCallback)
		pWindow->windowFocusCallback(pWindow, focused);
}

void WindowIconifyCallback(GLFWwindow* window, int iconified) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} iconified {}", pWindow->GetName(), iconified);

	if (pWindow->windowIconifyCallback)
		pWindow->windowIconifyCallback(pWindow, iconified);
}

void WindowMaximizeCallback(GLFWwindow* window, int maximize) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} maximized {}", pWindow->GetName(), maximize);

	pWindow->SetMaximized(maximize);

	if (pWindow->windowMaximizeCallback)
		pWindow->windowMaximizeCallback(pWindow, maximize);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} framebuffer resized to {}x{}", pWindow->GetName(), width, height);
	
	if (width != 0 && height != 0) {
		pWindow->AddFramesToDraw(1);
	}
	pWindow->SetFramebufferResized(true);
	// LOG_WINDOW("Window {} framebuffer resized to {}x{}", pWindow->GetName(), width, height);
	
	if (pWindow->framebufferSizeCallback) 
		pWindow->framebufferSizeCallback(pWindow, width, height);
}

void WindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_WINDOW("Window {} content scale changed to {}x{}", pWindow->GetName(), xscale, yscale);

	if (pWindow->windowContentScaleCallback)
		pWindow->windowContentScaleCallback(pWindow, xscale, yscale);
}



} // namespace WindowCallbacks

namespace _InputCallbacks {

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_INPUT("Window {} mouse button: {}, action: {}, mods: {}", pWindow->GetName(), button, action, mods);

	mouse.buttons[button] = action;
	mouse.mods = static_cast<GLFW::Mod>(mods);

	if (pWindow->mouseButtonCallback)
		pWindow->mouseButtonCallback(pWindow, button, action, mods);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_INPUT("Window {} cursor pos: {}, {}", pWindow->GetName(), xpos, ypos);

	mouse.deltaPos = mouse.pos - Lmath::vec2(xpos, ypos);
	mouse.pos = Lmath::vec2(xpos, ypos);
	// LOG_INPUT("Window {} delta mouse pos: {}, {}", pWindow->GetName(), pWindow->deltaMousePos.x, pWindow->deltaMousePos.y);

	// Drag window
	// if (glfwGetMouseButton(window, GLFW::MOUSE_BUTTON_RIGHT) == GLFW::PRESS) {
	// 	pWindow->CmdSetPos(pWindow->GetPos().x - mouse.deltaPos.x, pWindow->GetPos().y - mouse.deltaPos.y);
	// 	mouse.pos = mouse.pos + mouse.deltaPos;
	// }

	if (pWindow->cursorPosCallback)
		pWindow->cursorPosCallback(pWindow, xpos, ypos);
}

void CursorEnterCallback(GLFWwindow* window, int entered) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_INPUT("Window {} cursor entered: {}", pWindow->GetName(), entered);

	if (pWindow->cursorEnterCallback)
		pWindow->cursorEnterCallback(pWindow, entered);
}

void ScrollCallback(GLFWwindow* window, double x, double y) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_INPUT("Window {} scroll: {}, {}", pWindow->GetName(), x, y);

	mouse.scroll += y;
	mouse.deltaScroll += y;

	if (pWindow->scrollCallback)
		pWindow->scrollCallback(pWindow, x, y);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_INPUT("Window {} key: {}, scancode: {}, action: {}, mods: {}", pWindow->GetName(), key, scancode, action, mods);

	auto typed_key = static_cast<GLFW::Key>(key);

	if (action == GLFW::Press) {
		switch (typed_key) {
		// case GLFW::Key::ESCAPE:
		// 	pWindow->SetShouldClose(true);
		// 	break;
		case GLFW::Key::F11: {
			auto mode = pWindow->GetMode();
			// LOG_INFO("Window {} mode: {}", pWindow->GetName(), (int)mode);
			if (mode == WindowMode::WindowedFullScreen) {
				pWindow->SetMode(WindowMode::Windowed);
			} else {
				pWindow->StoreWindowSize();
				pWindow->SetMode(WindowMode::WindowedFullScreen);
			}}
			break;
		case GLFW::Key::F8 : {
			pWindow->SetDecorated(!pWindow->GetDecorated());
		}
			break;
		default:
			break;
		}

	}

	if (pWindow->keyCallback)
		pWindow->keyCallback(pWindow, key, scancode, action, mods);
}

void CharCallback(GLFWwindow* window, unsigned int codepoint) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_INPUT("Window {} char: {}", pWindow->GetName(), codepoint);

	if (pWindow->charCallback)
		pWindow->charCallback(pWindow, codepoint);
}

void CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_INPUT("Window {} char: {}, mods: {}", pWindow->GetName(), codepoint, mods);

	if (pWindow->charModsCallback)
		pWindow->charModsCallback(pWindow, codepoint, mods);
}


void DropCallback (GLFWwindow *window, int path_count, const char *paths[]) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	LOG_INPUT("Window {} drop: {}", pWindow->GetName(), path_count);
	
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
	glfwWindowHint(GLFW::ClientApi, GLFW::NoApi);
	
	// // ctx.monitors = glfwGetMonitors(&ctx.monitorCount);
	// glfwGetVideoModes(ctx.monitors[monitorIndex], &videoModeIndex);
	// videoModeIndex -= 1;
	is_initialized = true;
}

// Call after all graphics contexts are destroyed
void WindowManager::Finish() {
	if (!is_initialized) return;
	glfwTerminate();
	is_initialized = false;
}

// Window factory
Window::Window(int width, int height, const char* name, void* imguiStyle): name(name), size( width, height ) {

	if (!WindowManager::is_initialized) {
		WindowManager::Init();
	}

	glfwWindowHint(GLFW::Resizable, GLFW::True);
	window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	// LOG_INFO("Window::Create({}x{}):{} {}", width, height, name, (void*)window);
	GetPos();
	auto vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	sizeLimits = { 30, 30, vidMode->width, vidMode->height };
	glfwSetWindowSizeLimits(window, sizeLimits.x, sizeLimits.y, sizeLimits.z, sizeLimits.w);

	glfwSetWindowUserPointer(window, this);

	glfwSetWindowPosCallback         (window, _WindowCallbacks::WindowPosCallback         );
	glfwSetWindowSizeCallback        (window, _WindowCallbacks::WindowSizeCallback        );
	glfwSetWindowCloseCallback       (window, _WindowCallbacks::WindowCloseCallback       );
	glfwSetWindowRefreshCallback     (window, _WindowCallbacks::WindowRefreshCallback     );
	glfwSetWindowFocusCallback       (window, _WindowCallbacks::WindowFocusCallback       );
	glfwSetWindowIconifyCallback     (window, _WindowCallbacks::WindowIconifyCallback     );
	glfwSetWindowMaximizeCallback    (window, _WindowCallbacks::WindowMaximizeCallback    );
	glfwSetFramebufferSizeCallback   (window, _WindowCallbacks::FramebufferSizeCallback   );
	glfwSetWindowContentScaleCallback(window, _WindowCallbacks::WindowContentScaleCallback);

	glfwSetMouseButtonCallback       (window, _InputCallbacks::MouseButtonCallback);
	glfwSetCursorPosCallback         (window, _InputCallbacks::CursorPosCallback  );
	glfwSetScrollCallback            (window, _InputCallbacks::ScrollCallback     );
	glfwSetKeyCallback               (window, _InputCallbacks::KeyCallback        );
	glfwSetCharCallback              (window, _InputCallbacks::CharCallback       );
	glfwSetCharModsCallback          (window, _InputCallbacks::CharModsCallback   );
	glfwSetDropCallback              (window, _InputCallbacks::DropCallback       );

	UIContext.Init(imguiStyle); 

	ApplyChanges();
}

void Window::Destroy() {
	swapChain.Destroy();
	glfwGetWindowPos(window, &pos.x, &pos.y);
	UIContext.Destroy();
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
		framebufferResized = true;
		framesToDraw += 1;
		
		int monitorCount;
		auto monitors = glfwGetMonitors(&monitorCount);
		ASSERT(monitorIndex < monitorCount, "Invalid monitorIndex inside Window creation!", monitorIndex);
		auto monitor = monitors[monitorIndex];

		auto monitorMode = glfwGetVideoMode(monitor);
		
		switch (newMode) {
		case WindowMode::Windowed:
			// posY = std::max(posY, 31);
			glfwSetWindowMonitor(window, nullptr, windowedSize.x, windowedSize.y, windowedSize.z, windowedSize.w, GLFW::DontCare);
			if (maximized) {
				glfwMaximizeWindow(window);
			}
			// glfwSetWindowAttrib(window, GLFW::MAXIMIZED, maximized);
			glfwSetWindowAttrib(window, GLFW::Resizable, resizable);
			glfwSetWindowAttrib(window, GLFW::Decorated, decorated);
			break;
			case WindowMode::WindowedFullScreen:
			
			glfwWindowHint(GLFW::RedBits, monitorMode->redBits);
			glfwWindowHint(GLFW::GreenBits, monitorMode->greenBits);
			glfwWindowHint(GLFW::BlueBits, monitorMode->blueBits);
			glfwWindowHint(GLFW::RefreshRate, monitorMode->refreshRate);
			glfwSetWindowMonitor(window, monitor, 0, 0, monitorMode->width, monitorMode->height, monitorMode->refreshRate);
			break;
		case WindowMode::FullScreen:
			int modesCount;
			const GLFWvidmode* videoModes = glfwGetVideoModes(monitors[monitorIndex], &modesCount);
			if (videoModeIndex >= modesCount) {
				videoModeIndex = modesCount - 1;
			}
			GLFWvidmode videoMode = videoModes[videoModeIndex];
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode.width, videoMode.height, videoMode.refreshRate);
			break;
		}
	}

	// framebufferResized = false;
}



void Window::Update() {
	WINDOW_ALIVE_GUARD
	// for (int i = GLFW::Key::SPACE; i < GLFW::Key::LAST + 1; i++) {
	// 	lastKeyState[i] = glfwGetKey(window, i);
	// }
	// deltaScroll = 0;
	auto newTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
	deltaTime /= 1000.0f;
	lastTime = newTime;
	// double x, y;
	// glfwGetCursorPos(window, &x, &y);
	// deltaMousePos = mousePos - Lmath::vec2(x, y);
	// mousePos = Lmath::vec2(x, y);
}

std::string VideoModeText(GLFWvidmode mode) {
	return std::to_string(mode.width) + "x" + std::to_string(mode.height) + " " + std::to_string(mode.refreshRate) + " Hz";
}

// void Window::OnImgui(){}

void Window::UpdateFramebufferSize() {
	WINDOW_ALIVE_GUARD
	// framebufferResized = false;
	glfwGetFramebufferSize(window, &size.x, &size.y);
}

// bool Window::IsKeyPressed(uint16_t keyCode) {
// 	return lastKeyState[keyCode] && !glfwGetKey(window, keyCode);
// }
