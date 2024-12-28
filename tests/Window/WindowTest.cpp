#ifdef USE_MODULES
import lmath;
import vulkan_backend;
import window;
#else
#include "lmath.hpp"
#include "vulkan_backend.hpp"
#include "Window.cppm"
#endif

#include "Pch.hpp"
#include "Bindless.h"
#include "FileManager.hpp"

#include "WindowTest.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"


#include <unistd.h>

using namespace lmath;

using Pixel = vec4;
namespace {
struct Context {
	vb::Pipeline pipeline;
	// vb::Pipeline computePipeline;

	uint32_t width, height;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	// vb::Buffer outputImage;
	vb::Buffer vertexBuffer;
	
	// vb::Image renderImage;

	Window* mainWindow;
	std::set<Window*> windows;
	std::unordered_map<Window*, vb::Image> renderImages;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;


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
	pipeline = vb::CreatePipeline({
		.point = vb::PipelinePoint::Graphics,
		.stages = {
			{.stage = vb::ShaderStage::Vertex, .path = "tests/WindowTest/WindowTest.vert"},
			{.stage = vb::ShaderStage::Fragment, .path = "tests/WindowTest/WindowTest.frag"},
		},
		.name = "Window pipeline",
		// pos2 + color3
		.vertexAttributes = {vb::Format::RG32_sfloat, vb::Format::RGB32_sfloat},
		// .colorFormats = {ctx.albedo.format, ctx.normal.format, ctx.material.format, ctx.emission.format},
		.colorFormats = {vb::Format::RGBA16_sfloat},
		// .colorFormats = {vb::Format::RGBA8_unorm},
		.useDepth = false,
		// .depthFormat = {ctx.depth.format}
	});

}


void CreateRenderImage(Window* window) {
	// uint32_t width = window->GetMonitorWidth();
	// uint32_t height = window->GetMonitorHeight();
	uint32_t width = 3000;
	uint32_t height = 3000;
	ctx.renderImages.erase(window);
	ctx.renderImages.try_emplace(window, vb::CreateImage({
		.width = width,
		.height = height,
		.format = vb::Format::RGBA16_sfloat,
		// .format = vb::Format::RGBA8_unorm,
		.usage = vb::ImageUsage::ColorAttachment | vb::ImageUsage::TransferSrc | vb::ImageUsage::TransferDst,
		.name = window->GetName(),
	}));
}


void RecreateFrameResources(Window* w);
void UploadBuffers();

void FramebufferCallback(Window* window, int width, int height);
void MouseButtonCallback(Window* window, int button, int action, int mods);
void KeyCallback(Window* window, int key, int scancode, int action, int mods);



void UploadBuffers() {
	auto cmd = vb::GetCommandBuffer(vb::Queue::Transfer);
	cmd.BeginCommandBuffer();
	cmd.Copy(ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	cmd.Barrier();
	cmd.EndCommandBuffer();
	cmd.QueueSubmit();
}

void RecordCommands(Window* window) {
	auto size = window->GetSize();
	auto glfwWindow = window->GetGLFWwindow();
	vec4 viewport = {0, 0, (float)size.x, (float)size.y};
	WindowTestConstants constants{};
	constants.transform = float4((float)size.y / size.x, 0.0f,
								0.0f, float(size.y) / size.x);
	constants.offset = {0.0f, 0.0f};

	// LOG_INFO("Viewport: {}, {}, {}, {}", viewport.x, viewport.y, viewport.z, viewport.w); 

	auto cmd = window->swapChain.GetCommandBuffer();
	cmd.BeginCommandBuffer();
	if (!window->swapChain.AcquireImage()) return;
	vb::Image& img = window->swapChain.GetCurrentImage();
	
	// cmd.Copy(ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	cmd.Barrier(ctx.renderImages[window], {vb::ImageLayout::TransferDst});
	cmd.ClearColorImage(ctx.renderImages[window], {0.7f, 0.0f, 0.4f, 1.0f});

	cmd.BeginRendering({ctx.renderImages[window]}, {}, 1, viewport);
	cmd.BindPipeline(ctx.pipeline);
	cmd.PushConstants(&constants, sizeof(constants));
	cmd.BindVertexBuffer(ctx.vertexBuffer);
	cmd.Draw(3, 1, 0, 0);
	cmd.EndRendering();
	
	cmd.Barrier(ctx.renderImages[window], {vb::ImageLayout::TransferSrc});
	cmd.Barrier(img, {vb::ImageLayout::TransferDst});
	cmd.Blit(img, ctx.renderImages[window], {0, 0, size.x, size.y}, {0, 0, size.x, size.y});
	cmd.Barrier(img, {vb::ImageLayout::Present});

}

static int frameCount = 0;
void DrawWindow(Window* window) {
	if (window->GetDrawNeeded() && !window->GetIconified()) {
		RecordCommands(window);
		if (window->swapChain.GetDirty()) {
			LOG_WARN("RecordCommands: Swapchain dirty");
		}
		window->swapChain.SubmitAndPresent();
		if (window->swapChain.GetDirty()) {
			LOG_WARN("SubmitAndPresent: Swapchain dirty");
		}
		window->SetDrawNeeded(false);
		frameCount = (frameCount + 1) % (1 << 15);
	}
}

void KeyCallback(Window* window, int key, int scancode, int action, int mods)
{
	// printf("key: %d, scancode: %d, action: %d, mods: %d\n", key, scancode, action, mods);
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_N: {
			static int windowCount = 1;
			std::string name = "w" + std::to_string(windowCount++);
			auto window = WindowManager::NewWindow(ctx.width, ctx.height, name.c_str());
			CreateRenderImage(window);
			ctx.windows.emplace(window);
			window->AddFramebufferSizeCallback(FramebufferCallback);
			window->AddMouseButtonCallback(MouseButtonCallback);
			window->AddKeyCallback(KeyCallback);
		}
		default:
			break;
		}

	}
}


void MouseButtonCallback(Window* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		window->SetResizable(!window->GetResizable());
	}
}

void FramebufferCallback(Window* window, int width, int height) {
	// DEBUG_TRACE("Window {} framebuffer resized to {}x{}", window->GetName(), width, height);
	if (width == 0 || height == 0) {return;}
	// LOG_INFO("RecreateFrameResources {} callback", window->GetName());
	RecreateFrameResources(window);
	DrawWindow(window);
}

void RecreateFrameResources(Window* window) {
	if (!window->GetAlive()) { /* LOG_WARN("RecreateFrameResources: Window is dead"); */ return;} // important
	if (window->GetIconified()) {/* LOG_TRACE("RecreateFrameResources: size = 0"); */ return;};

	vb::WaitIdle();
	bool swapChainDirty = window->GetSwapchainDirty();
	bool framebufferResized = window->GetFramebufferResized();
	// LOG_INFO("RecreateFrameResources {} {} {} {}", window->GetName(), (void*)window->GetGLFWwindow(), swapChainDirty, framebufferResized);
	if (swapChainDirty || framebufferResized) {
		// LOG_INFO("DIRTY FRAME RESOURCES");
		window->UpdateFramebufferSize();
		window->GetSwapchain().Recreate(window->GetWidth(), window->GetHeight());
		window->SetFramebufferResized(false);
		window->SetDrawNeeded(true);
	}

	// }
	// ivec2 size = w->GetSize();
	// if (size.x > ctx.renderImage.width || size.y > ctx.renderImage.height) {
	// 	ctx.CreateImages(size.x, size.y);
	// }
	// camera->extent = {viewportSize.x, viewportSize.y};
}

} // namespace

void WindowTestApplication::run(WindowTestInfo* pWindowTestInfo) {
	info = pWindowTestInfo;
	Setup();
	Create();
	MainLoop();
	// Draw();
	Finish();
}

void WindowTestApplication::Setup() {
	ctx.width = info->width;
	ctx.height = info->height;
}

void WindowTestApplication::Create() {
	vb::Init();
	auto window = WindowManager::NewWindow(ctx.width, ctx.height, "wm");
	ctx.windows.emplace(window);
	ctx.mainWindow = window;
	// ctx.window1 = WindowManager::NewWindow(ctx.width, ctx.height, "w1");
	window->AddFramebufferSizeCallback(FramebufferCallback);
	window->AddMouseButtonCallback(MouseButtonCallback);
	window->AddKeyCallback(KeyCallback);
	// window->SetMaxSize(3000, 3000);
	// ctx.CreateImages(window->GetMonitorWidth(), window->GetMonitorHeight());
	CreateRenderImage(window);
	ctx.vertexBuffer = vb::CreateBuffer(vertices.size() * sizeof(Vertex), vb::BufferUsage::Vertex, vb::Memory::GPU, "Vertex Buffer");
	UploadBuffers();
	ctx.CreateShaders();
}

static int loopCount = 0;
void WindowTestApplication::MainLoop() {
	Context* c = &ctx;
	while (ctx.mainWindow != nullptr) {
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
				delete window;
				continue;
			}
			// LOG_INFO("RecreateFrameResources {}", window->GetName());
			RecreateFrameResources(window);
			++it;
		}
		for (auto& window : ctx.windows) {
            DrawWindow(window);
        }
	}
	vb::WaitIdle();
}

void WindowTestApplication::Finish() {
	ctx = {};
	vb::Destroy();
	WindowManager::Finish();
}