#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "SlangTest.hpp"
#include "Lmath.hpp"
#include "Timer.hpp"

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;

	int width, height;
	int learningRate;

	vkw::Buffer ImageGT;
	vkw::Buffer ImageOpt;
	vkw::Buffer grad;

	vkw::Buffer bufferCPU;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;
}
void Context::CreateShaders() {
	pipeline = vkw::CreatePipeline({
		.point = vkw::PipelinePoint::Compute,
		.stages = {
			{.stage = vkw::ShaderStage::Compute, .path = "tests/SlangTest/clearColor.slang"},
		},
		.name = "Slang Test",
	});
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.ImageGT = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::GPU, "Ground Truth Image");
	ctx.ImageOpt = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferSrc, vkw::Memory::GPU, "Optimized Image");
	ctx.grad = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage, vkw::Memory::GPU, "Gradient Image");
	ctx.bufferCPU = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::CPU, "Output Image");
}

static void fillRGB(float4* image, int width, int height){
	for (auto i = 0; i < height; i++) {
		for (auto j = 0; j < width; j++) {
			int idx = i * width + j;
			image[idx].r = i / (float)height;
			image[idx].g = j / (float)width;
			image[idx].b = 0.0f;
			image[idx].a = 1.0f;
		}
	}
}

static void saveBuffer(const char *fname, vkw::Buffer& buffer, uint32_t width, uint32_t height) {
	std::vector<unsigned char> image;
	image.reserve(width * height * 4);
	Pixel* mappedMemory = (Pixel*)vkw::MapBuffer(buffer);
	for (int i = 0; i < width * height; i++) {
		image.push_back(255.0f * mappedMemory[i].r);
		image.push_back(255.0f * mappedMemory[i].g);
		image.push_back(255.0f * mappedMemory[i].b);
		image.push_back(255.0f);
	}
	vkw::UnmapBuffer(buffer);
	FileManager::SaveBMP(fname, (const uint32_t*)image.data(), width, height);
}

void SlangTestApplication::run(SlangTestInfo* pSlangTestInfo) {
	info = pSlangTestInfo;
	Setup();
	Create();
	Compute();
	Finish();
}

void SlangTestApplication::Setup() {
	ctx.width = info->width; 
	ctx.height = info->height;
	ctx.learningRate = info->learningRate;
}

void SlangTestApplication::Create() {
	vkw::Init();
	ctx.CreateImages(ctx.width, ctx.height);
	ctx.CreateShaders();
}


void SlangTestApplication::Compute() {
	Timer timer;
	std::vector<float4> imageUV(ctx.width * ctx.height);
	fillRGB(imageUV.data(), ctx.width, ctx.height);
	vkw::BeginCommandBuffer(vkw::Queue::Compute);
	vkw::CmdBindPipeline(ctx.pipeline);

	ImageOptConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.imageOptRID = ctx.ImageOpt.RID();
	constants.imageGTRID = ctx.ImageGT.RID();
	constants.gradRID = ctx.grad.RID();
	constants.learningRate = ctx.learningRate;

	vkw::CmdPushConstants(&constants, sizeof(constants));

	vkw::CmdDispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});
	vkw::CmdBarrier();
	vkw::CmdCopy(ctx.bufferCPU, ctx.ImageOpt, ctx.width * ctx.height * sizeof(Pixel));

	timer.Start();
	vkw::EndCommandBuffer();
	vkw::WaitQueue(vkw::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("output.bmp", ctx.bufferCPU, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void SlangTestApplication::Finish() {
	ctx = {};
	vkw::Destroy();
}
