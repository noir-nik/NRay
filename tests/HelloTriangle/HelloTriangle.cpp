#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "HelloTriangle.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

#include "Window.hpp"

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

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;
}
void Context::CreateShaders() {
	pipeline = vkw::CreatePipeline({
		.point = vkw::PipelinePoint::Compute,
		.stages = {
			// {.stage = vkw::ShaderStage::Compute, .path = "tests/HelloTriangle/HelloTriangle.slang"},
			{.stage = vkw::ShaderStage::Compute, .path = "tests/HelloTriangle/HelloTriangle.comp"},
		},
		.name = "Slang Test",
	});
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.outputImage = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage, vkw::Memory::CPU, "Output Image");
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

	vkw::BeginCommandBuffer(vkw::Queue::Compute);
	vkw::CmdBindPipeline(ctx.pipeline);
	vkw::CmdPushConstants(&constants, sizeof(constants));

	vkw::CmdDispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});

	timer.Start();
	vkw::EndCommandBuffer();
	vkw::WaitQueue(vkw::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("HelloTriangle.bmp", &ctx.outputImage, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void HelloTriangleApplication::Finish() {
	ctx = {};
	vkw::Destroy();
}
