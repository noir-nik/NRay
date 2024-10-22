#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "FeatureTest.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

#include "Window.hpp"

#include <unistd.h>

using namespace Lmath;

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;
	// vkw::Pipeline computePipeline;

	uint32_t width, height;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	// vkw::Buffer outputImage;
	vkw::Buffer vertexBuffer;
	
	// vkw::Image renderImage;

	Window* mainWindow;
	std::set<Window*> windows;
	std::unordered_map<Window*, vkw::Image> renderImages;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;
}

struct Vertex {
	vec2 pos;
	vec3 color;
};

const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
};

void Context::CreateShaders() {
	pipeline = vkw::CreatePipeline({
		.point = vkw::PipelinePoint::Graphics,
		.stages = {
			{.stage = vkw::ShaderStage::Vertex, .path = "tests/FeatureTest/FeatureTest.vert"},
			{.stage = vkw::ShaderStage::Fragment, .path = "tests/FeatureTest/FeatureTest.frag"},
		},
		.name = "Hello triangle pipeline",
		// pos2 + color3
		.vertexAttributes = {vkw::Format::RG32_sfloat, vkw::Format::RGB32_sfloat},
		// .colorFormats = {ctx.albedo.format, ctx.normal.format, ctx.material.format, ctx.emission.format},
		.colorFormats = {vkw::Format::RGBA16_sfloat},
		// .colorFormats = {vkw::Format::RGBA8_unorm},
		.useDepth = false,
		// .depthFormat = {ctx.depth.format}
	});

}

// void Context::CreateImages(uint32_t width, uint32_t height) {
	
// }

void CreateRenderImage(Window* window) {
	// uint32_t width = window->GetMonitorWidth();
	// uint32_t height = window->GetMonitorHeight();
	uint32_t width = 3000;
	uint32_t height = 3000;
	ctx.renderImages.erase(window);
	ctx.renderImages.try_emplace(window, vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA16_sfloat,
		// .format = vkw::Format::RGBA8_unorm,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc | vkw::ImageUsage::TransferDst,
		.name = window->GetName(),
	}));
}

void FeatureTestApplication::run(FeatureTestInfo* pFeatureTestInfo) {
	info = pFeatureTestInfo;
	Setup();
	Create();
	MainLoop();
	// Draw();
	Finish();
}

void RecreateFrameResources(Window* w);
void UploadBuffers();

void FramebufferCallback(GLFWwindow* window, int width, int height);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void FeatureTestApplication::Setup() {
	ctx.width = info->width;
	ctx.height = info->height;
}

void FeatureTestApplication::Create() {
	auto window = WindowManager::NewWindow(ctx.width, ctx.height, "wm");
	ctx.windows.emplace(window);
	ctx.mainWindow = window;
	// ctx.window1 = WindowManager::NewWindow(ctx.width, ctx.height, "w1");
	vkw::Init(window->GetGLFWwindow(), window->GetWidth(), window->GetHeight());
	window->SetFramebufferSizeCallback(FramebufferCallback);
	window->SetMouseButtonCallback(MouseButtonCallback);
	window->SetKeyCallback(KeyCallback);
	window->SetMaxSize(3000, 3000);
	// ctx.CreateImages(window->GetMonitorWidth(), window->GetMonitorHeight());
	CreateRenderImage(window);
	ctx.vertexBuffer = vkw::CreateBuffer(vertices.size() * sizeof(Vertex), vkw::BufferUsage::Vertex, vkw::Memory::GPU, "Vertex Buffer");
	UploadBuffers();
	ctx.CreateShaders();
}

void UploadBuffers() {
	auto cmd = vkw::GetCommandBuffer(vkw::Queue::Transfer);
	vkw::BeginCommandBuffer(cmd);
	vkw::CmdCopy(cmd, ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	vkw::CmdBarrier(cmd);
	vkw::EndCommandBuffer(cmd);
	vkw::QueueSubmit({cmd});
}

void RecordCommands(Window* window) {
	auto size = window->GetSize();
	auto glfwWindow = window->GetGLFWwindow();
	vec4 viewport = {0, 0, (float)size.x, (float)size.y};
	FeatureTestConstants constants{};
	constants.transform = float4((float)size.y / size.x, 0.0f,
								0.0f, float(size.y) / size.x);
	constants.offset = {0.0f, 0.0f};

	// LOG_INFO("Viewport: {}, {}, {}, {}", viewport.x, viewport.y, viewport.z, viewport.w); 

	auto cmd = vkw::GetCommandBuffer(glfwWindow);
	vkw::BeginCommandBuffer(cmd);
	if (!vkw::AcquireImage(glfwWindow)) return;
	vkw::Image& img = vkw::GetCurrentSwapchainImage(glfwWindow);
	
	// vkw::CmdCopy(cmd, ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	vkw::CmdBarrier(cmd, ctx.renderImages[window], vkw::Layout::TransferDst);
	vkw::CmdClearColorImage(cmd, ctx.renderImages[window], {0.7f, 0.0f, 0.4f, 1.0f});

	vkw::CmdBeginRendering(cmd, {ctx.renderImages[window]}, {}, 1, viewport);
	vkw::CmdBindPipeline(cmd, ctx.pipeline);
	vkw::CmdPushConstants(cmd, &constants, sizeof(constants));
	vkw::CmdBindVertexBuffer(cmd, ctx.vertexBuffer);
	vkw::CmdDraw(cmd, 3, 1, 0, 0);
	vkw::CmdEndRendering(cmd);
	
	vkw::CmdBarrier(cmd, ctx.renderImages[window], vkw::Layout::TransferSrc);
	vkw::CmdBarrier(cmd, img, vkw::Layout::TransferDst);
	vkw::CmdBlit(cmd, img, ctx.renderImages[window], {0, 0, size.x, size.y}, {0, 0, size.x, size.y});
	vkw::CmdBarrier(cmd, img, vkw::Layout::Present);

}

static int frameCount = 0;
void DrawWindow(Window* window) {
	if (window->GetDrawNeeded() && !window->GetIconified()) {
		auto glfwWindow = window->GetGLFWwindow();
		RecordCommands(window);
		if (vkw::GetSwapChainDirty(glfwWindow)) {
			LOG_WARN("RecordCommands: Swapchain dirty");
		}
		vkw::SubmitAndPresent(glfwWindow);
		if (vkw::GetSwapChainDirty(glfwWindow)) {
			LOG_WARN("SubmitAndPresent: Swapchain dirty");
		}
		window->SetDrawNeeded(false);
		frameCount = (frameCount + 1) % (1 << 15);
	}
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// printf("key: %d, scancode: %d, action: %d, mods: %d\n", key, scancode, action, mods);
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			pWindow->SetShouldClose(true);
			break;
		case GLFW_KEY_F11: {
			auto moden = pWindow->GetMode();
			// LOG_INFO("Window {} mode: {}", pWindow->GetName(), (int)mode);
			if (pWindow->GetMode() == WindowMode::WindowedFullScreen) {
				pWindow->SetMode(WindowMode::Windowed);
			} else {
				pWindow->StoreWindowSize();
				pWindow->SetMode(WindowMode::WindowedFullScreen);
			}}
			break;
		case GLFW_KEY_N: {
			static int windowCount = 1;
			std::string name = "w" + std::to_string(windowCount++);
			auto window = WindowManager::NewWindow(ctx.width, ctx.height, name.c_str());
			CreateRenderImage(window);
			ctx.windows.emplace(window);
			window->SetFramebufferSizeCallback(FramebufferCallback);
			window->SetMouseButtonCallback(MouseButtonCallback);
			window->SetKeyCallback(KeyCallback);
			window->SetMaxSize(3000, 3000);
		}
		default:
			break;
		}

	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		pWindow->SetResizable(!pWindow->GetResizable());
	}
	
}

void FramebufferCallback(GLFWwindow* window, int width, int height) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetSize(width, height);
	pWindow->SetSwapchainDirty(true);
	pWindow->SetDrawNeeded(true);
	DEBUG_TRACE("Window {} framebuffer resiznnned to {}x{}", pWindow->GetName(), width, height);
	if (width == 0 || height == 0) {return;}
	LOG_INFO("RecreateFrameResources {} callback", pWindow->GetName());
	RecreateFrameResources(pWindow);
	DrawWindow(pWindow);
}


static int loopCount = 0;
void FeatureTestApplication::MainLoop() {
	Context* c = &ctx;
	while (ctx.mainWindow != nullptr) {
		for (auto& window : ctx.windows) {LOG_INFO("Drawing window {}", window->GetName()); DrawWindow(window);}
		WindowManager::WaitEvents();
		
		for (auto it = ctx.windows.begin(); it != ctx.windows.end();) {
			auto window = *it;
			window->ApplyChanges();
			if (!window->GetAlive()) {
				ctx.renderImages.erase(window);
				it = ctx.windows.erase(it);
				if (window == ctx.mainWindow) {
					ctx.mainWindow = nullptr;
				}
				vkw::DestroySwapChain(window->GetGLFWwindow());
				delete window;
				continue;
			}
			// LOG_INFO("RecreateFrameResources {}", window->GetName());
			RecreateFrameResources(window);
			++it;
		}
		// printf("Loop\n");
		// fflush(stdout);
		LOG_INFO("Loop {}", loopCount++);
	}
	vkw::WaitIdle();
}
/* 
void FeatureTestApplication::Draw() {
	Timer timer;
	// constants.storageImageRID = ctx.renderImage.RID();
	GLFWwindow* window = ctx.window->GetGLFWwindow();

	auto cmd = vkw::GetCommandBuffer(window);
	vkw::BeginCommandBuffer(cmd);
	// vkw::CmdPushConstants(&constants, sizeof(constants));
	// vkw::CmdBeginPresent();
	vkw::AcquireImage(window);
	vkw::Image& img = vkw::GetCurrentSwapchainImage(window);
	
	vkw::CmdCopy(cmd, ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	vkw::CmdBarrier(cmd, ctx.renderImage, vkw::Layout::TransferDst);
	vkw::CmdClearColorImage(cmd, ctx.renderImage, {0.7f, 0.0f, 0.4f, 1.0f});

	vkw::CmdBeginRendering(cmd, {ctx.renderImage});
	vkw::CmdBindPipeline(cmd, ctx.pipeline);
	vkw::CmdBindVertexBuffer(cmd, ctx.vertexBuffer);
	vkw::CmdDraw(cmd, 3, 1, 0, 0);
	vkw::CmdEndRendering(cmd);
	
	vkw::CmdBarrier(cmd, ctx.renderImage, vkw::Layout::TransferSrc);
	vkw::CmdBarrier(cmd, img, vkw::Layout::TransferDst);
	vkw::CmdBlit(cmd, img, ctx.renderImage, {}, {});

	vkw::CmdBarrier(cmd, img, vkw::Layout::Present);
	vkw::SubmitAndPresent(window);
	vkw::WaitQueue(vkw::Queue::Graphics);
	sleep(3);
}
 */
void FeatureTestApplication::Finish() {
	for (auto& window: ctx.windows) {vkw::DestroySwapChain(window->GetGLFWwindow()); delete window;}
	ctx = {};
	vkw::Destroy();
	WindowManager::Finish();
}

void RecreateFrameResources(Window* window) {
	if (!window->GetAlive()) { LOG_WARN("RecreateFrameResources: Window is dead"); return;} // important
	if (window->GetIconified()) {LOG_TRACE("RecreateFrameResources: size = 0"); return;};

	vkw::WaitIdle();
	bool swapChainDirty = vkw::GetSwapChainDirty(window->GetGLFWwindow());
	bool windowDirty = window->GetSwapchainDirty();
	LOG_INFO("RecreateFrameResources {} {} {} {}", window->GetName(), (void*)window->GetGLFWwindow(), swapChainDirty, windowDirty);
	if (swapChainDirty || windowDirty) {
		LOG_INFO("DIRTY FRAME RESOURCES");
		window->UpdateFramebufferSize();
		vkw::RecreateSwapChain(window->GetGLFWwindow(), window->GetWidth(), window->GetHeight());
		window->SetSwapchainDirty(false);
		window->SetDrawNeeded(true);
	}

	// }
	// ivec2 size = w->GetSize();
	// if (size.x > ctx.renderImage.width || size.y > ctx.renderImage.height) {
	// 	ctx.CreateImages(size.x, size.y);
	// }
	// camera->extent = {viewportSize.x, viewportSize.y};
}
