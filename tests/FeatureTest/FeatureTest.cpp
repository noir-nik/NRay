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
	
	std::shared_ptr<Window> window;
	// GLFWwindow* glfwWindow = nullptr;
	std::shared_ptr<Window> window1;

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
			{.stage = vkw::ShaderStage::Vertex, .path = "tests/HelloTriangle/HelloTriangle.vert"},
			{.stage = vkw::ShaderStage::Fragment, .path = "tests/HelloTriangle/HelloTriangle.frag"},
		},
		.name = "Hello triangle pipeline",
		// pos2 + color3
		.vertexAttributes = {vkw::Format::RG32_sfloat, vkw::Format::RGB32_sfloat},
		// .colorFormats = {ctx.albedo.format, ctx.normal.format, ctx.material.format, ctx.emission.format},
		.colorFormats = {vkw::Format::RGBA16_sfloat},
		.useDepth = false,
		// .depthFormat = {ctx.depth.format}
	});

}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.vertexBuffer = vkw::CreateBuffer(vertices.size() * sizeof(Vertex), vkw::BufferUsage::Vertex, vkw::Memory::GPU, "Vertex Buffer");

	ctx.renderImage = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA16_sfloat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc | vkw::ImageUsage::TransferDst,
		.name = "Render Image",
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

void FeatureTestApplication::Setup() {
	ctx.width = info->width;
	ctx.height = info->height;
}

void FeatureTestApplication::Create() {
	ctx.window = WindowManager::NewWindow(ctx.width, ctx.height, "w0");
	// ctx.glfwWindow = ctx.window->GetGLFWwindow();
	// ctx.window1 = WindowManager::NewWindow(ctx.width, ctx.height, "w1");
	vkw::Init(ctx.window->GetGLFWwindow(), ctx.window->GetWidth(), ctx.window->GetHeight());
	ctx.CreateImages(ctx.width, ctx.height);
	ctx.CreateShaders();
}

void RecreateFrameResources(Window* w);

void RenderFrame(GLFWwindow* window) {
	
	auto cmd = vkw::GetCommandBuffer(window);
	vkw::BeginCommandBuffer(cmd);
	// vkw::CmdPushConstants(&constants, sizeof(constants));
	// vkw::CmdBeginPresent();
	if (!vkw::AcquireImage(window)) return;
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
}


void DrawFrame(GLFWwindow* window) {
	RenderFrame(window);
	if (vkw::GetSwapChainDirty(window)) {
		LOG_WARN("DrawFrame: Swapchain dirty");
		return;
	}
	vkw::SubmitAndPresent(window);
}

bool SwapchainDirty(Window* w) {
	bool dirty = false;
	auto swapChainDirty = vkw::GetSwapChainDirty(w->GetGLFWwindow());
	auto windowDirty = w->GetSwapchainDirty();
	LOG_INFO("swapChainDirty: {}, windowDirty: {}", swapChainDirty, windowDirty);
	dirty = swapChainDirty || windowDirty;
	return dirty;
}

void FeatureTestApplication::MainLoop() {
	while (ctx.window->GetAlive() && !ctx.window->GetShouldClose()) {
		ctx.window->Update();
		if (/* engine->drawNeeded || */ ctx.window->GetDrawNeeded()) {
			DrawFrame(ctx.window->GetGLFWwindow());
			/* engine->redrawNeeded = false; */
			ctx.window->SetDrawNeeded(false);
			continue;
		}
		// usleep(1000 * 1000);
		// ctx.window->SetSize(800, 600); 
		if (SwapchainDirty(ctx.window.get())) {
			LOG_INFO("DIRTY FRAME RESOURCES");
			RecreateFrameResources(ctx.window.get());
		}
		WindowManager::WaitEvents();
		printf("Loop\n");
		fflush(stdout);
	}


	// DrawFrame(ctx.window->GetGLFWwindow());
	// usleep(1000 * 1000);
	// ctx.window->SetSize(800, 600); 
	// // ctx.window->ApplyChanges();
	// if (SwapchainDirty(ctx.window.get())) {
	// 	LOG_INFO("DIRTY FRAME RESOURCES");
	// 	RecreateFrameResources(ctx.window.get());
	// }
	// usleep(1000 * 1000);
	// DrawFrame(ctx.window->GetGLFWwindow());
	// usleep(1000 * 1000);



	// DrawFrame(ctx.window->GetGLFWwindow());
	// usleep(1000 * 1000);

	vkw::WaitIdle();
}

void FeatureTestApplication::Draw() {
	Timer timer;
	FeatureTestConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
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

void FeatureTestApplication::Finish() {
	ctx = {};
	vkw::Destroy();
	
	WindowManager::Finish();
}

void RecreateFrameResources(Window* w) {
	// busy wait while the window is minimized
	while (w->GetWidth() == 0 || w->GetHeight() == 0) {
		WindowManager::WaitEvents();
	}
	vkw::WaitIdle();
	if (w->GetFramebufferResized() || w->IsDirty()) {
		if (w->IsDirty()) {
			LOG_INFO("RecreateFrameResources: Dirty");
			w->ApplyChanges();
		}
		if (w->GetFramebufferResized()) {
			LOG_INFO("RecreateFrameResources: FramebufferResized");
		}
		if (w->GetAlive()) {
			w->UpdateFramebufferSize();
			vkw::OnSurfaceUpdate(w->GetGLFWwindow(), w->GetWidth(), w->GetHeight());
		} else {
			LOG_WARN("RecreateFrameResources: Window is dead");
			return;
		}
	}
	// DeferredRenderer::CreateImages(viewportSize.x, viewportSize.y);
	// camera->extent = {viewportSize.x, viewportSize.y};
}
