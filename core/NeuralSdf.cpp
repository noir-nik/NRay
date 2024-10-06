#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "NeuralSdf.hpp"

struct Context {
    vkw::Pipeline forwardPipeline;

    std::unordered_map<std::string, int> shaderVersions;
    // vkw::Image outputImage;
	vkw::Buffer weightsGPU;
	vkw::Buffer outputImage;

	int width, height;
	int numLayers, layerSize;
	int num_parameters;
	int activations_size;
	int zs_size;

};
Context ctx;

void CreatePipeline(vkw::Pipeline& pipeline, const vkw::PipelineDesc& desc) {
	bool should_update = false;
	for (auto& stage : desc.stages) {
		auto path = "source/Shaders/" + stage.path.string();
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
		pipeline = vkw::CreatePipeline(desc);
	}
}

void CreateShaders() {
    CreatePipeline(ctx.forwardPipeline, {
        .point = vkw::PipelinePoint::Compute,
        .stages = {
            {.stage = vkw::ShaderStage::Compute, .path = "neuralSdfTest.comp"},
        },
        .name = "Neural Sdf Forward",
    });
}

void CreateImages(uint32_t width, uint32_t height) {
	ctx.weightsGPU = vkw::CreateBuffer(ctx.num_parameters * sizeof(float), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::GPU, "Neural Sdf Weights");
	ctx.outputImage = vkw::CreateBuffer(width * height, vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::CPU, "Output Image");
}

void NeuralSdfApplication::run(NeuralSdfInfo* pNeuralSdfInfo) {
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
	weights = FileManager::ReadFloats(info->weightsPath);
	ASSERT(weights.size() == ctx.num_parameters, "Invalid number of weights, expected {0}, got {1}", ctx.num_parameters, weights.size());
}

void NeuralSdfApplication::Create() {
		// TODO: vkw::requestCompute();
		vkw::Init();
		CreateImages(ctx.width, ctx.height);
		CreateShaders();
	}

void NeuralSdfApplication::Compute() {
		vkw::BeginCommandBuffer(vkw::Queue::Compute);
		vkw::CmdBindPipeline(ctx.forwardPipeline);
		NeuralSdfConstants constants;
		constants.width = ctx.width;
		constants.height = ctx.height;
		constants.numLayers = ctx.numLayers;
		constants.layerSize = ctx.layerSize;
		vkw::CmdPushConstants(&ctx, sizeof(constants));
	}

void NeuralSdfApplication::MainLoop() {
	}

void NeuralSdfApplication::Finish() {
		Destroy();
		vkw::Destroy();
	}
