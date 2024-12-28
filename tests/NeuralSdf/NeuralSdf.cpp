#ifdef USE_MODULES
import lmath;
import vulkan_backend;
#else
#include "lmath.hpp"
#include "vulkan_backend.hpp"
#include "Pch.hpp"

#endif
#include "Bindless.h"
#include "FileManager.hpp"

#include "NeuralSdf.hpp"

#include "Timer.hpp"

using Pixel = vec4;
namespace{
struct Context {
	vb::Pipeline glslPipeline;
	vb::Pipeline slangPipeline;

	std::unordered_map<std::string, int> shaderVersions;
	vb::Buffer weightsGPU;
	vb::Buffer outputImage;

	vb::Buffer bufferCPUglsl;
	vb::Buffer bufferCPUslang;

	int width, height;
	int numLayers, layerSize;
	int num_parameters;
	int activations_size;
	int zs_size;

};
static Context ctx;
}
void CreatePipeline(vb::Pipeline& pipeline, const vb::PipelineDesc& desc) {
	bool should_update = false;
	for (auto& stage : desc.stages) {
		auto path = stage.path.string();
		auto it = ctx.shaderVersions.find(path);
		auto version = FileManager::GetFileVersion(path);
		// TODO: Check for -1 (file not found)~
		ASSERT(version != -1, "File not found: " + path);

		// not found or outdated
		if (it == ctx.shaderVersions.end() || version > it->second) {
			ctx.shaderVersions[path] = version;
			should_update = true;
		}
	}
	if (should_update) {
		pipeline = vb::CreatePipeline(desc);
	}
}

void CreateShaders() {
	CreatePipeline(ctx.glslPipeline, {
		.point = vb::PipelinePoint::Compute,
		.stages = {
			// {.stage = vb::ShaderStage::Compute, .path = "clearColor.slang"},
			{.stage = vb::ShaderStage::Compute, .path = "tests/NeuralSdf/neuralSDF.comp"},
		},
		.name = "Neural Sdf Glsl",
	});

	CreatePipeline(ctx.slangPipeline, {
		.point = vb::PipelinePoint::Compute,
		.stages = {
			// {.stage = vb::ShaderStage::Compute, .path = "clearColor.slang"},
			{.stage = vb::ShaderStage::Compute, .path = "tests/NeuralSdf/neuralSDF.slang"},
		},
		.name = "Neural Sdf Slang",
	});
		
}

void CreateImages(uint32_t width, uint32_t height) {
	ctx.outputImage = vb::CreateBuffer(width * height * sizeof(Pixel), vb::BufferUsage::Storage | vb::BufferUsage::TransferSrc, vb::Memory::GPU, "Output Image");
	ctx.weightsGPU = vb::CreateBuffer(ctx.num_parameters * sizeof(float), vb::BufferUsage::Storage | vb::BufferUsage::TransferDst, vb::Memory::GPU, "Neural Sdf Weights");
	// ctx.imageGPU = vb::CreateImage({
	// 	.width = width,
	// 	.height = height,
	// 	.format = vb::Format::RGBA32_sfloat,
	// 	// .format = vb::Format::RGBA8_unorm,
	// 	.usage = vb::ImageUsage::ColorAttachment | vb::ImageUsage::TransferDst | vb::ImageUsage::TransferSrc | vb::ImageUsage::Storage,
	// 	.name = "imageGPU"
	// });

	ctx.bufferCPUglsl = vb::CreateBuffer(width * height * sizeof(Pixel), vb::BufferUsage::Storage | vb::BufferUsage::TransferDst, vb::Memory::CPU, "bufferCPUglsl");
	ctx.bufferCPUslang = vb::CreateBuffer(width * height * sizeof(Pixel), vb::BufferUsage::Storage | vb::BufferUsage::TransferDst, vb::Memory::CPU, "bufferCPUslang");
}



static void saveBuffer(char const *fname, vb::Buffer& buffer, uint32_t width, uint32_t height) {
	std::vector<unsigned char> image;
	image.reserve(width * height * 4);
	Pixel* mappedMemory = (Pixel*)vb::MapBuffer(buffer);
	for (int i = 0; i < width * height; i++) {
		image.push_back(255.0f * mappedMemory[i].r);
		image.push_back(255.0f * mappedMemory[i].g);
		image.push_back(255.0f * mappedMemory[i].b);
		image.push_back(255.0f);
	}
	vb::UnmapBuffer(buffer);
	FileManager::SaveBMP(fname, (const uint32_t*)image.data(), width, height);
}

void NeuralSdfApplication::run(NeuralSdfInfo* pNeuralSdfInfo) {
	info = pNeuralSdfInfo;
	Setup();
	Create();
	Compute();
	Finish();
}

void NeuralSdfApplication::Setup() {
	ctx.numLayers = info->numLayers;
	ctx.layerSize = info->layerSize;
	ctx.num_parameters = (3*ctx.layerSize + ctx.numLayers*ctx.layerSize*ctx.layerSize + ctx.layerSize) + (ctx.layerSize*(ctx.numLayers + 1) + 1);
	ctx.activations_size = 3 + (ctx.numLayers + 1)*ctx.layerSize;
	ctx.zs_size = (ctx.numLayers + 1)*ctx.layerSize + 1;
	ctx.width = info->width; 
	ctx.height = info->height;
	// Read Weights
	FileManager::ReadFloats(info->weightsPath, weights);
	ASSERT(weights.size() == ctx.num_parameters, "Invalid number of weights, expected {0}, got {1}", ctx.num_parameters, weights.size());
}

void NeuralSdfApplication::Create() {
	// TODO: vb::requestCompute();
	vb::Init();
	CreateImages(ctx.width, ctx.height);
	CreateShaders();
}

void NeuralSdfApplication::Compute() {
	Timer timer;
	NeuralSdfConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.numLayers = ctx.numLayers;
	constants.layerSize = ctx.layerSize;
	constants.weightsRID = ctx.weightsGPU.RID();
	constants.outputImageRID = ctx.outputImage.RID();

	auto cmd = vb::GetCommandBuffer(vb::Queue::Compute);
	cmd.BeginCommandBuffer();

	cmd.Copy(ctx.weightsGPU, weights.data(), ctx.num_parameters * sizeof(float));
	// Glsl shader
	cmd.BindPipeline(ctx.glslPipeline);
	constants.outputImageRID = ctx.bufferCPUglsl.RID();
	cmd.PushConstants(&constants, sizeof(constants));
	cmd.Dispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});
	cmd.Barrier();

	// Slang shader
	cmd.BindPipeline(ctx.slangPipeline);
	constants.outputImageRID = ctx.bufferCPUslang.RID();
	cmd.PushConstants(&constants, sizeof(constants));
	cmd.Dispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});

	timer.Start();
	cmd.EndCommandBuffer();
	vb::WaitQueue(vb::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("neuralSdfglsl.bmp", ctx.bufferCPUglsl, ctx.width, ctx.height);
	saveBuffer("neuralSdslang.bmp", ctx.bufferCPUslang, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void NeuralSdfApplication::Finish() {
	ctx = {};
	weights.clear();
	vb::Destroy();
}
