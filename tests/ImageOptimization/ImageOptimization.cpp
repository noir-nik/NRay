#ifdef USE_MODULES
import lmath;
import vulkan_backend;
#else
#include "lmath_types.hpp"
#include <vulkan_backend/core.hpp>
#endif

#include "Bindless.h"
#include "FileManager.cppm"

#include "ImageOptimization.cppm"
#include "../TestCommon.cppm"
#include "Timer.cppm"

using Pixel = vec4;
namespace {
struct Context {
	vb::Pipeline pipeline;

	int width, height;
	float learningRate;
	int numIterations;

	vb::Buffer BufferGT;
	vb::Buffer BufferOpt;
	vb::Buffer grad;

	vb::Buffer bufferCPU;
	
	vb::Image imageCPU;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;
}

void Context::CreateShaders() {
	pipeline = vb::CreatePipeline({
		.point = vb::PipelinePoint::Compute,
		.stages = {
			{.stage = vb::ShaderStage::Compute, .path = "tests/ImageOptimization/ImageOptimization.slang"},
			// {.stage = vb::ShaderStage::Compute, .path = "tests/ImageOptimization/ImageOptimization.comp"},
		},
		.name = "Image Optimization Test",
	});
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.BufferGT = vb::CreateBuffer(width * height * sizeof(Pixel), vb::BufferUsage::Storage | vb::BufferUsage::TransferDst, vb::Memory::GPU, "Ground Truth Image");
	ctx.BufferOpt = vb::CreateBuffer(width * height * sizeof(Pixel), vb::BufferUsage::Storage | vb::BufferUsage::TransferSrc | vb::BufferUsage::TransferDst, vb::Memory::GPU, "Optimized Image");
	ctx.grad = vb::CreateBuffer(width * height * sizeof(Pixel), vb::BufferUsage::Storage, vb::Memory::GPU, "Gradient Image");
	ctx.bufferCPU = vb::CreateBuffer(width * height * sizeof(Pixel), vb::BufferUsage::Storage | vb::BufferUsage::TransferDst, vb::Memory::CPU, "Output Image");
	ctx.imageCPU = vb::CreateImage({
		.width = width,
		.height = height,
		.format = vb::Format::RGBA32_sfloat,
		.usage = vb::ImageUsage::ColorAttachment | vb::ImageUsage::TransferSrc | vb::ImageUsage::TransferDst,
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
	vb::Init();
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

	auto cmd = vb::GetCommandBuffer(vb::Queue::Compute);
	cmd.BeginCommandBuffer();
	// Prepare BufferGT and BufferOpt
	cmd.Copy(ctx.BufferGT, imageUV.data(), ctx.width * ctx.height * sizeof(Pixel));
	cmd.Barrier(ctx.imageCPU, {vb::ImageLayout::General});
	cmd.ClearColorImage(ctx.imageCPU, {0.0f, 0.0f, 0.5f, 0.0f});
	cmd.Barrier(ctx.imageCPU, {vb::ImageLayout::TransferSrc});
	cmd.Copy(ctx.BufferOpt, ctx.imageCPU);
	cmd.Barrier();

	cmd.BindPipeline(ctx.pipeline);
	cmd.PushConstants(&constants, sizeof(constants));

	cmd.Dispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});
	cmd.Barrier();
	cmd.Copy(ctx.bufferCPU, ctx.BufferOpt, ctx.width * ctx.height * sizeof(Pixel));

	timer.Start();
	cmd.EndCommandBuffer();
	vb::WaitQueue(vb::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("imageOpt.bmp", &ctx.bufferCPU, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void ImageOptimizationApplication::Finish() {
	ctx = {};
	vb::Destroy();
}
