#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "NeuralSdf.hpp"

struct Context {
    vkw::Pipeline forwardPipeline;

    std::unordered_map<std::string, int> shaderVersions;
    // vkw::Image outputImage;
	vkw::Buffer weights;
	vkw::Buffer outputImage;

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
            {.stage = vkw::ShaderStage::Compute, .path = "neuralSdfForward.comp"},
        },
        .name = "Neural Sdf Forward",
    });
}

void CreateImages(uint32_t width, uint32_t height) {
	ctx.weights = vkw::CreateBuffer(ctx.num_parameters, vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::GPU, "Neural Sdf Weights");
	ctx.outputImage = vkw::CreateBuffer(width * height, vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::GPU | vkw::Memory::CPU, "Neural Sdf Output Image");
}

void NeuralSdfApplication::run(NeuralSdfInfo* pNeuralSdfInfo) {
	Setup();
	Create();
	Compute();
	Finish();
}

void NeuralSdfApplication::Setup() {
	int layerSize = info->layerSize;
	int numLayers = info->numLayers;
	ctx.num_parameters = (3*layerSize + numLayers*layerSize*layerSize + layerSize) + (layerSize*(numLayers + 1) + 1);
	ctx.activations_size = 3 + (numLayers + 1)*layerSize;
	ctx.zs_size = (numLayers + 1)*layerSize + 1;
	// Read Weights
	weights = FileManager::ReadFloats(info->weightsPath);
	ASSERT(weights.size() == ctx.num_parameters, "Invalid number of weights, expected {0}, got {1}", ctx.num_parameters, weights.size());
	
}

void NeuralSdfApplication::Create() {
		// TODO: vkw::requestCompute();
		vkw::Init();
		ImageOptimizer::CreateImages(width, height);
		ImageOptimizer::CreateShaders();
	}

void NeuralSdfApplication::Compute() {
		vkw::BeginCommandBuffer(vkw::Queue::Compute);
	}

void NeuralSdfApplication::MainLoop() {
	}

void NeuralSdfApplication::Finish() {
		ImageOptimizer::Destroy();
		vkw::Destroy();
	}
