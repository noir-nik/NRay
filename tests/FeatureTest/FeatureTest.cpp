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
	
	vkw::Image renderImage;
	
	// std::shared_ptr<Window> window;
	// GLFWwindow* glfwWindow = nullptr;
	// std::shared_ptr<Window> window1;

	std::vector<std::shared_ptr<Window>> windows;

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

void Context::CreateImages(uint32_t width, uint32_t height) {
	static int count = 0;
	// ctx.renderImage.resource.reset();
	ctx.renderImage = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA16_sfloat,
		// .format = vkw::Format::RGBA8_unorm,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc | vkw::ImageUsage::TransferDst,
		.name = "Render Image" + std::to_string(count++),
	});
}

void FeatureTestApplication::run(FeatureTestInfo* pFeatureTestInfo) {
	info = pFeatureTestInfo;
	Setup();
	Create();
	MainLoop();
	// Draw();
	Finish();
}

bool SwapchainDirty(Window* w);
void RecreateFrameResources(Window* w);
void UploadBuffers();

void FramebufferCallback(GLFWwindow* window, int width, int height);



void FeatureTestApplication::Setup() {
	ctx.width = info->width;
	ctx.height = info->height;
}

void FeatureTestApplication::Create() {
	ctx.window = WindowManager::NewWindow(ctx.width, ctx.height, "w0");
	// ctx.glfwWindow = ctx.window->GetGLFWwindow();
	// ctx.window1 = WindowManager::NewWindow(ctx.width, ctx.height, "w1");
	vkw::Init(ctx.window->GetGLFWwindow(), ctx.window->GetWidth(), ctx.window->GetHeight());
	ctx.window->SetFramebufferSizeCallback(FramebufferCallback);
	ctx.window->SetMaxSize(3000, 3000);
	ctx.CreateImages(ctx.window->GetMonitorWidth(), ctx.window->GetMonitorHeight());
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

void RecordCommands(GLFWwindow* window) {
	auto size = ctx.window->GetSize();
	vec4 viewport = {0, 0, (float)size.x, (float)size.y};
	FeatureTestConstants constants{};
	constants.transform = float4((float)size.y / size.x, 0.0f,
								0.0f, float(size.y) / size.x);
	constants.offset = {0.0f, 0.0f};

	// LOG_INFO("Viewport: {}, {}, {}, {}", viewport.x, viewport.y, viewport.z, viewport.w); 


	auto cmd = vkw::GetCommandBuffer(window);
	vkw::BeginCommandBuffer(cmd);
	if (!vkw::AcquireImage(window)) return;
	vkw::Image& img = vkw::GetCurrentSwapchainImage(window);
	
	// vkw::CmdCopy(cmd, ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	vkw::CmdBarrier(cmd, ctx.renderImage, vkw::Layout::TransferDst);
	vkw::CmdClearColorImage(cmd, ctx.renderImage, {0.7f, 0.0f, 0.4f, 1.0f});

	vkw::CmdBeginRendering(cmd, {ctx.renderImage}, {}, 1, viewport);
	vkw::CmdBindPipeline(cmd, ctx.pipeline);
	vkw::CmdPushConstants(cmd, &constants, sizeof(constants));
	vkw::CmdBindVertexBuffer(cmd, ctx.vertexBuffer);
	vkw::CmdDraw(cmd, 3, 1, 0, 0);
	vkw::CmdEndRendering(cmd);
	
	vkw::CmdBarrier(cmd, ctx.renderImage, vkw::Layout::TransferSrc);
	vkw::CmdBarrier(cmd, img, vkw::Layout::TransferDst);
	vkw::CmdBlit(cmd, img, ctx.renderImage, {0, 0, size.x, size.y}, {0, 0, size.x, size.y});
	vkw::CmdBarrier(cmd, img, vkw::Layout::Present);
}

int DrawFrame(GLFWwindow* window) {
	RecordCommands(window);
	if (vkw::GetSwapChainDirty(window)) {
		LOG_WARN("RecordCommands: Swapchain dirty");
		return 0;
	}
	vkw::SubmitAndPresent(window);
	if (vkw::GetSwapChainDirty(window)) {
		LOG_WARN("SubmitAndPresent: Swapchain dirty");
		return 0;
	}
	return 1;
}
Timer t;
void FramebufferCallback(GLFWwindow* window, int width, int height) {
	Window* pWindow = (Window*)glfwGetWindowUserPointer(window);
	pWindow->SetSize(width, height);
	pWindow->SetSwapchainDirty(true);
	pWindow->SetDrawNeeded(true);
	DEBUG_TRACE("Window {} framebuffer resized to {}x{}", pWindow->GetName(), width, height);
	if (width == 0 || height == 0) {
		return;
	}
	auto start = glfwGetTime();
	if (SwapchainDirty(ctx.window.get())) {
		LOG_INFO("DIRTY FRAME RESOURCES");
		RecreateFrameResources(ctx.window.get());
		ctx.window->SetSwapchainDirty(false);
	}
	// LOG_ERROR("Recr: {0:.6f} fps {1}", t.Elapsed(), 1.0f / t.Elapsed());
	auto end = glfwGetTime();
	auto delta = end - start;
	LOG_ERROR("Recr: {0:.6f} fps {1}", delta, 1.0f / delta)
	// t.Start();
	start = glfwGetTime();
	if (/* engine->drawNeeded || */ ctx.window->GetDrawNeeded()) {
		// LOG_ERROR("DRAW FRAME");
		auto drawn = DrawFrame(ctx.window->GetGLFWwindow());
		// LOG_TRACE("drawn: {}", drawn);
		if (drawn) {
			ctx.window->SetDrawNeeded(false);
			/* engine->redrawNeeded = false; */
			// LOG_ERROR("Draw: {0:.6f} fps {1}", t.Elapsed(), 1.0f / t.Elapsed());
			auto end = glfwGetTime();
			auto delta = end - start;
			LOG_ERROR("Draw: {0:.6f} fps {1}", delta, 1.0f / delta);
		}
	}
	


	// ctx.window->ApplyChanges();
	
}

void DrawWindows(){
	for (auto& window : ctx.windows) {
		bool drawPossible = window->GetWidth() > 0 && window->GetHeight() > 0;
		if (window->GetDrawNeeded() && drawPossible) {
			auto drawn = DrawFrame(window->GetGLFWwindow());
			LOG_TRACE("drawn: {}", drawn);
			if (drawn) {window->SetDrawNeeded(false);}
		}
	}
}

void FeatureTestApplication::MainLoop() {
	while (ctx.window->GetAlive() && !ctx.window->GetShouldClose()) {
		DrawWindows();
		WindowManager::WaitEvents();
		// ctx.window->ApplyChanges();
		if (SwapchainDirty(ctx.window.get())) {
			LOG_INFO("DIRTY FRAME RESOURCES");
			RecreateFrameResources(ctx.window.get());
				ctx.window->SetSwapchainDirty(false);
		}
		// printf("Loop\n");
		fflush(stdout);
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
	ctx = {};
	vkw::Destroy();
	
	WindowManager::Finish();
}

bool SwapchainDirty(Window* w) {
	bool dirty = false;
	auto swapChainDirty = vkw::GetSwapChainDirty(w->GetGLFWwindow());
	auto windowDirty = w->GetSwapchainDirty();
	// LOG_INFO("swapChainDirty: {}, windowDirty: {}", swapChainDirty, windowDirty);
	dirty = swapChainDirty || windowDirty;
	return dirty;
}


void RecreateFrameResources(Window* w) {
	// busy wait while the window is minimized
	if (w->GetWidth() == 0 || w->GetHeight() == 0) {LOG_ERROR("RecreateFrameResources: size = 0"); return;};
	// while (w->GetWidth() == 0 || w->GetHeight() == 0) {
	// 	DEBUG_TRACE("Iconified");
	// 	WindowManager::WaitEvents();
	// }
	vkw::WaitIdle();
	if (w->GetAlive()) {
		w->UpdateFramebufferSize();
		vkw::OnSurfaceUpdate(w->GetGLFWwindow(), w->GetWidth(), w->GetHeight());
	} else {
		LOG_WARN("RecreateFrameResources: Window is dead");
		return;
	}
	// }
	ivec2 size = {w->GetWidth(), w->GetHeight()};
	if (size.x > ctx.renderImage.width || size.y > ctx.renderImage.height) {
		ctx.CreateImages(size.x, size.y);
	}
	// camera->extent = {viewportSize.x, viewportSize.y};
}
