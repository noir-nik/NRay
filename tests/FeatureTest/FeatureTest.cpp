#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "FeatureTest.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;

	int width, height;

	vkw::Buffer BufferGT;
	vkw::Buffer BufferOpt;
	vkw::Buffer grad;

	vkw::Buffer bufferCPU;
	
	vkw::Image imageCPU;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;
}
void Context::CreateShaders() {
	pipeline = vkw::CreatePipeline({
		.point = vkw::PipelinePoint::Compute,
		.stages = {
			{.stage = vkw::ShaderStage::Compute, .path = "tests/FeatureTest/FeatureTest.slang"},
		},
		.name = "Slang Test",
	});
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.BufferGT = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::GPU, "Ground Truth Image");
	ctx.BufferOpt = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferSrc | vkw::BufferUsage::TransferDst, vkw::Memory::GPU, "Optimized Image");
	ctx.grad = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage, vkw::Memory::GPU, "Gradient Image");
	ctx.bufferCPU = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::CPU, "Output Image");
	ctx.imageCPU = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA32_sfloat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc | vkw::ImageUsage::TransferDst,
		.name = "Output Image",
	});
}

void FeatureTestApplication::run(FeatureTestInfo* pFeatureTestInfo) {
	info = pFeatureTestInfo;
	Setup();
	Create();
	Compute();
	Finish();
}

void FeatureTestApplication::Setup() {
	ctx.width = info->width; 
	ctx.height = info->height;
}

void FeatureTestApplication::Create() {
	vkw::Init();
	ctx.CreateImages(ctx.width, ctx.height);
	ctx.CreateShaders();
}

void FeatureTestApplication::Compute() {
	Timer timer;
	FeatureTestConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.imageOptRID = ctx.BufferOpt.RID();
	constants.imageGTRID = ctx.BufferGT.RID();
	constants.gradRID = ctx.grad.RID();

	std::vector<float4> imageUV(ctx.width * ctx.height);
	fillUV(imageUV.data(), ctx.width, ctx.height);

	vkw::BeginCommandBuffer(vkw::Queue::Compute);
	// Prepare BufferGT and BufferOpt
	vkw::CmdCopy(ctx.BufferGT, imageUV.data(), ctx.width * ctx.height * sizeof(Pixel));
	vkw::CmdBarrier(ctx.imageCPU, vkw::Layout::General);
	vkw::CmdClearColorImage(ctx.imageCPU, {0.0f, 0.0f, 0.5f, 0.0f});
	vkw::CmdBarrier(ctx.imageCPU, vkw::Layout::TransferSrc);
	vkw::CmdCopy(ctx.BufferOpt, ctx.imageCPU);
	vkw::CmdBarrier();

	vkw::CmdBindPipeline(ctx.pipeline);
	vkw::CmdPushConstants(&constants, sizeof(constants));

	vkw::CmdDispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});
	vkw::CmdBarrier();
	vkw::CmdCopy(ctx.bufferCPU, ctx.BufferOpt, ctx.width * ctx.height * sizeof(Pixel));

	timer.Start();
	vkw::EndCommandBuffer();
	vkw::WaitQueue(vkw::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("imageOpt.bmp", &ctx.bufferCPU, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void FeatureTestApplication::Finish() {
	ctx = {};
	vkw::Destroy();
}
