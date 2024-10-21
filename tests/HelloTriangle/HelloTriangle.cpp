#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "HelloTriangle.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

#include "Window.hpp"

#include <unistd.h>

using namespace Lmath;

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;
	vkw::Pipeline computePipeline;

	uint32_t width, height;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	// vkw::Buffer outputImage;
	vkw::Buffer vertexBuffer;

	vkw::Image renderImage;

	Window* window;

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
        .colorFormats = {vkw::Format::RGBA32_sfloat},
        .useDepth = false,
        // .depthFormat = {ctx.depth.format}
    });

	computePipeline = vkw::CreatePipeline({
		.point = vkw::PipelinePoint::Compute,
		.stages = {
			{.stage = vkw::ShaderStage::Compute, .path = "tests/HelloTriangle/testDraw.comp"},
		},
		.name = "TestDraw Compute Pipeline",
	});
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	// ctx.outputImage = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage, vkw::Memory::CPU, "Output Image");
	ctx.vertexBuffer = vkw::CreateBuffer(vertices.size() * sizeof(Vertex), vkw::BufferUsage::Vertex, vkw::Memory::GPU, "Vertex Buffer");

	ctx.renderImage = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA32_sfloat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc | vkw::ImageUsage::TransferDst | vkw::ImageUsage::Storage,
		.name = "Render Image",
	});
}

void HelloTriangleApplication::run(HelloTriangleInfo* pHelloTriangleInfo) {
	info = pHelloTriangleInfo;
	Setup();
	Create();
	Draw();
	// MainLoop();
	Finish();
}

void HelloTriangleApplication::Setup() {
	ctx.width = info->width; 
	ctx.height = info->height;
}

void HelloTriangleApplication::Create() {
	ctx.window = WindowManager::NewWindow(ctx.width, ctx.height, "Feature Test");
	vkw::Init(ctx.window->GetGLFWwindow(), ctx.window->GetWidth(), ctx.window->GetHeight());
	ctx.CreateImages(ctx.width, ctx.height);
	ctx.CreateShaders();
}

// void HelloTriangleApplication::MainLoop() {
// 	Timer timer;
// 	for (int i = 0; i < 5000; i++) {
// 		auto r = sinf(0.5f * i+ 1);
// 		auto g = cosf(0.9f * i);
// 		vkw::BeginCommandBuffer(vkw::Queue::Graphics);
// 		// vkw::CmdBeginRendering(attachs, ctx.depth);
// 		// vkw::CmdBindPipeline(ctx.pipeline);
// 		// vkw::CmdPushConstants(&constants, sizeof(constants));
// 		vkw::AcquireImage();
// 		vkw::Image& img = vkw::GetCurrentSwapchainImage(ctx.window.get()->GetGLFWwindow());
// 		vkw::CmdBarrier(img, vkw::Layout::TransferDst);
// 		vkw::CmdClearColorImage(img, {std::abs(r), 0.5f, 0.0f, 1.0f});
// 		vkw::CmdEndPresent();
// 		// vkw::EndCommandBuffer();
// 		timer.Start();
// 		vkw::SubmitAndPresent();
// 		// vkw::WaitQueue(vkw::Queue::Graphics);
// 		printf("%d, Render time: %f\n", i, timer.Elapsed());
// 		// usleep(100 * 1000);
// 		// glfwWaitEvents();
// 	}
// 	LOG_INFO("Waiting for 1 second...");
// 	vkw::WaitQueue(vkw::Queue::Graphics);
// 	usleep(1000 * 1000);
// }

void HelloTriangleApplication::Draw() {
	Timer timer;
	HelloTriangleConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.storageImageRID = ctx.renderImage.RID();
	
	auto cmd = vkw::GetCommandBuffer(ctx.window->GetGLFWwindow());
	vkw::BeginCommandBuffer(cmd);
	// vkw::CmdPushConstants(&constants, sizeof(constants));
	GLFWwindow* window = ctx.window->GetGLFWwindow();
	// vkw::CmdBeginPresent();
	vkw::AcquireImage(window);
	vkw::Image& img = vkw::GetCurrentSwapchainImage(window);
	vkw::CmdBarrier(cmd, ctx.renderImage, vkw::Layout::ColorAttachment);
	vkw::CmdCopy(cmd, ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	// vkw::CmdClearColorImage(cmd, ctx.renderImage, {0.7f, 0.0f, 0.4f, 1.0f});

	vkw::CmdBindPipeline(cmd, ctx.computePipeline);
	vkw::CmdPushConstants(cmd, &constants, sizeof(constants));
	vkw::CmdDispatch(cmd, {(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});

	vkw::CmdBeginRendering(cmd, {ctx.renderImage});
	vkw::CmdBindPipeline(cmd, ctx.pipeline);
	vkw::CmdBindVertexBuffer(cmd, ctx.vertexBuffer);
	vkw::CmdDraw(cmd, 3, 1, 0, 0);
	vkw::CmdEndRendering(cmd);
	
	vkw::CmdBarrier(cmd, ctx.renderImage, vkw::Layout::TransferSrc);
	vkw::CmdBarrier(cmd, img, vkw::Layout::TransferDst);
	vkw::CmdBlit(cmd, img, ctx.renderImage);

	vkw::CmdBarrier(cmd, img, vkw::Layout::Present);
	vkw::SubmitAndPresent(window);
	vkw::WaitQueue(vkw::Queue::Graphics);
	sleep(3);
	// timer.Start();
	// getchar();
}


/* 
void HelloTriangleApplication::Draw() {
	Timer timer;
	HelloTriangleConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.outputImageRID = ctx.outputImage.RID();
	
	vkw::BeginCommandBuffer(vkw::Queue::Graphics);
	// vkw::CmdPushConstants(&constants, sizeof(constants));

	// vkw::CmdBeginPresent();
	vkw::AcquireImage();
	vkw::Image& img = vkw::GetCurrentSwapchainImage();
	vkw::CmdBarrier(img, vkw::Layout::TransferDst);

	vkw::CmdBarrier(ctx.renderImage, vkw::Layout::TransferDst);
	vkw::CmdClearColorImage(ctx.renderImage, {0.7f, 0.0f, 0.4f, 1.0f});
	vkw::CmdBarrier(ctx.renderImage, vkw::Layout::TransferSrc);

	vkw::CmdBlit(img, ctx.renderImage, {ctx.width, ctx.height}, {ctx.width, ctx.height});

	vkw::CmdBarrier(img, vkw::Layout::Present);

	// vkw::EndCommandBuffer();
	vkw::SubmitAndPresent();
	vkw::WaitQueue(vkw::Queue::Graphics);
	sleep(3);
	// timer.Start();
} */

void HelloTriangleApplication::Finish() {
	ctx = {};
	vkw::Destroy();
}
