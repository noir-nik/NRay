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
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
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
	ctx.window = WindowManager::NewWindow(ctx.width, ctx.height, "Feature Test");
	vkw::Init(ctx.window->GetGLFWwindow(), ctx.window->GetWidth(), ctx.window->GetHeight());
	// ctx.CreateImages(ctx.width, ctx.height);
	// ctx.CreateShaders();
}

void FeatureTestApplication::MainLoop() {
	// while (!ctx.window->GetShouldClose()) {
	// 	ctx.window->Update();
	// }
}

void FeatureTestApplication::Draw() {
	Timer timer;
	FeatureTestConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	// constants.storageImageRID = ctx.renderImage.RID();
	
	vkw::BeginCommandBuffer(vkw::Queue::Graphics);
	// vkw::CmdPushConstants(&constants, sizeof(constants));

	// vkw::CmdBeginPresent();
	vkw::AcquireImage();
	vkw::Image& img = vkw::GetCurrentSwapchainImage();
	vkw::CmdCopy(ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	vkw::CmdBarrier(ctx.renderImage, vkw::Layout::TransferDst);
	vkw::CmdClearColorImage(ctx.renderImage, {0.7f, 0.0f, 0.4f, 1.0f});

	vkw::CmdBeginRendering({ctx.renderImage});
	vkw::CmdBindPipeline(ctx.pipeline);
	vkw::CmdBindVertexBuffer(ctx.vertexBuffer);
	vkw::CmdDraw(3, 1, 0, 0);
	vkw::CmdEndRendering();
	
	vkw::CmdBarrier(ctx.renderImage, vkw::Layout::TransferSrc);
	vkw::CmdBarrier(img, vkw::Layout::TransferDst);
	vkw::CmdBlit(img, ctx.renderImage);

	vkw::CmdBarrier(img, vkw::Layout::Present);
	vkw::SubmitAndPresent();
	vkw::WaitQueue(vkw::Queue::Graphics);
	sleep(3);
	// timer.Start();
	// getchar();
}

void FeatureTestApplication::Finish() {
	ctx = {};
	vkw::Destroy();
	// WindowManager::Finish();
}