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
// 		// cmd.BeginRendering(ctx.depth);
// 		// cmd.BindPipeline(ipeline);
// 		// cmd.PushConstants(nstants, sizeof(constants));
// 		vkw::AcquireImage();
// 		vkw::Image& img = vkw::GetCurrentSwapchainImage(ctx.window.get()->GetGLFWwindow());
// 		cmd.Barrier(vkw::Layout::TransferDst);
// 		cmd.ClearColorImage({std::abs(r), 0.5f, 0.0f, 1.0f});
// 		cmd.EndPresent(/ 		// vkw::EndCommandBuffer();
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
	cmd.BeginCommandBuffer();
	// cmd.PushConstants(nstants, sizeof(constants));
	GLFWwindow* window = ctx.window->GetGLFWwindow();
	// cmd.BeginPresent(vkw::AcquireImage(window);
	vkw::Image& img = vkw::GetCurrentSwapchainImage(window);
	cmd.Barrier(ctx.renderImage, vkw::Layout::ColorAttachment);
	cmd.Copy(ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	// cmd.ClearColorImage(ctx.renderImage, {0.7f, 0.0f, 0.4f, 1.0f});

	cmd.BindPipeline(ctx.computePipeline);
	cmd.PushConstants(&constants, sizeof(constants));
	cmd.Dispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});

	cmd.BeginRendering({ctx.renderImage});
	cmd.BindPipeline(ctx.pipeline);
	cmd.BindVertexBuffer(ctx.vertexBuffer);
	cmd.Draw(3, 1, 0, 0);
	cmd.EndRendering();
	
	cmd.Barrier(ctx.renderImage, vkw::Layout::TransferSrc);
	cmd.Barrier(img, vkw::Layout::TransferDst);
	cmd.Blit(img, ctx.renderImage);

	cmd.Barrier(img, vkw::Layout::Present);
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
	// cmd.PushConstants(nstants, sizeof(constants));

	// cmd.BeginPresent(vkw::AcquireImage();
	vkw::Image& img = vkw::GetCurrentSwapchainImage();
	cmd.Barrier(vkw::Layout::TransferDst);

	cmd.Barrier(enderImage, vkw::Layout::TransferDst);
	cmd.ClearColorImage(enderImage, {0.7f, 0.0f, 0.4f, 1.0f});
	cmd.Barrier(enderImage, vkw::Layout::TransferSrc);

	cmd.Blit(ctx.renderImage, {ctx.width, ctx.height}, {ctx.width, ctx.height});

	cmd.Barrier(vkw::Layout::Present);

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
