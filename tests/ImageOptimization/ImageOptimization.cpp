#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "ImageOptimization.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;

	int width, height;
	float learningRate;
	int numIterations;

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
			{.stage = vkw::ShaderStage::Compute, .path = "tests/ImageOptimization/ImageOptimization.slang"},
			// {.stage = vkw::ShaderStage::Compute, .path = "tests/ImageOptimization/ImageOptimization.comp"},
		},
		.name = "Image Optimization Test",
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

void ImageOptimizationApplication::run(ImageOptimizationInfo* pImageOptimizationInfo) {
	info = pImageOptimizationInfo;
	Setup();
	Create();
	Compute();
	Finish();
}

void ImageOptimizationApplication::Setup() {
	ctx.width = info->width; 
	ctx.height = info->height;
	ctx.learningRate = info->learningRate;
	ctx.numIterations = info->numIterations;
}

void ImageOptimizationApplication::Create() {
	vkw::Init();
	ctx.CreateImages(ctx.width, ctx.height);
	ctx.CreateShaders();
}

void ImageOptimizationApplication::CmdOptimizationStep(){

}

void ImageOptimizationApplication::Compute() {
	Timer timer;
	ImageOptConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.imageOptRID = ctx.BufferOpt.RID();
	constants.imageGTRID = ctx.BufferGT.RID();
	constants.gradRID = ctx.grad.RID();
	constants.learningRate = ctx.learningRate;
	constants.numIterations = ctx.numIterations;

	std::vector<float4> imageUV(ctx.width * ctx.height);
	fillUV(imageUV.data(), ctx.width, ctx.height);

	auto cmd = vkw::GetCommandBuffer(vkw::Queue::Compute);
	cmd.BeginCommandBuffer();
	// Prepare BufferGT and BufferOpt
	cmd.Copy(ctx.BufferGT, imageUV.data(), ctx.width * ctx.height * sizeof(Pixel));
	cmd.Barrier(ctx.imageCPU, vkw::Layout::General);
	cmd.ClearColorImage(ctx.imageCPU, {0.0f, 0.0f, 0.5f, 0.0f});
	cmd.Barrier(ctx.imageCPU, vkw::Layout::TransferSrc);
	cmd.Copy(ctx.BufferOpt, ctx.imageCPU);
	cmd.Barrier();

	cmd.BindPipeline(ctx.pipeline);
	cmd.PushConstants(&constants, sizeof(constants));

	cmd.Dispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});
	cmd.Barrier();
	cmd.Copy(ctx.bufferCPU, ctx.BufferOpt, ctx.width * ctx.height * sizeof(Pixel));

	timer.Start();
	cmd.EndCommandBuffer();
	vkw::WaitQueue(vkw::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("imageOpt.bmp", &ctx.bufferCPU, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void ImageOptimizationApplication::Finish() {
	ctx = {};
	vkw::Destroy();
}
