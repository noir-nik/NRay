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

	int width, height;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	vkw::Buffer outputImage;
	vkw::Buffer vertexBuffer;

	vkw::Image imageCPU;

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
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

void Context::CreateShaders() {
	// pipeline = vkw::CreatePipeline({
    //     .point = vkw::PipelinePoint::Graphics,
    //     .stages = {
    //         {.stage = vkw::ShaderStage::Vertex, .path = "tests/HelloTriangle/HelloTriangle.vert"},
    //         {.stage = vkw::ShaderStage::Fragment, .path = "tests/HelloTriangle/HelloTriangle.frag"},
    //     },
    //     .name = "Opaque Pipeline",
    //     // .vertexAttributes = {vkw::Format::RGB32_sfloat, vkw::Format::RGB32_sfloat, vkw::Format::RGBA32_sfloat, vkw::Format::RG32_sfloat},
	// 	// pos2 + color3
    //     .vertexAttributes = {vkw::Format::RG32_sfloat, vkw::Format::RGB32_sfloat},
    //     // .colorFormats = {ctx.albedo.format, ctx.normal.format, ctx.material.format, ctx.emission.format},
    //     .useDepth = false,
    //     // .depthFormat = {ctx.depth.format}
    // });
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.outputImage = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage, vkw::Memory::CPU, "Output Image");
	ctx.vertexBuffer = vkw::CreateBuffer(vertices.size() * sizeof(Vertex), vkw::BufferUsage::Vertex, vkw::Memory::GPU, "Vertex Buffer");

	ctx.imageCPU = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA32_sfloat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc | vkw::ImageUsage::TransferDst,
		.name = "Output Image",
	});
}

void HelloTriangleApplication::run(HelloTriangleInfo* pHelloTriangleInfo) {
	info = pHelloTriangleInfo;
	Setup();
	Create();
	Draw();
	Finish();
}

void HelloTriangleApplication::Setup() {
	ctx.width = info->width; 
	ctx.height = info->height;
}

void HelloTriangleApplication::Create() {
	Window::Create();
	vkw::Init(Window::GetGLFWwindow(), Window::GetWidth(), Window::GetHeight());
	ctx.CreateImages(ctx.width, ctx.height);
	ctx.CreateShaders();
}


void HelloTriangleApplication::Draw() {
	Timer timer;
	HelloTriangleConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.outputImageRID = ctx.outputImage.RID();

	vkw::BeginCommandBuffer(vkw::Queue::Graphics);
	// vkw::CmdBeginRendering(attachs, ctx.depth);
	// vkw::CmdBindPipeline(ctx.pipeline);
	// vkw::CmdPushConstants(&constants, sizeof(constants));
	vkw::CmdBeginPresent();
	vkw::CmdBarrier(ctx.imageCPU, vkw::Layout::General);
	vkw::CmdClearColorImage(ctx.imageCPU, {0.7f, 0.0f, 0.4f, 1.0f});
	vkw::CmdEndPresent();
	timer.Start();
	vkw::EndCommandBuffer();
	vkw::SubmitAndPresent();
	vkw::WaitQueue(vkw::Queue::Graphics);
	printf("Compute time: %fs\n", timer.Elapsed());
	sleep(3);
	// timer.Start();
	// saveBuffer("HelloTriangle.bmp", &ctx.outputImage, ctx.width, ctx.height);
	// printf("Save time: %fs\n", timer.Elapsed());
}

void HelloTriangleApplication::Finish() {
	ctx = {};
	vkw::Destroy();
	Window::Destroy();
}
