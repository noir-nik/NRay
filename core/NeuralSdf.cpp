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
		auto path = "Shaders/" + stage.path.string();
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
	ctx.outputImage = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::CPU, "Output Image");
}


// void readFromBuffer(vkw::Buffer& buffer, void* data, size_t size) {
// 	vkw::M
// }

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
		vkw::CmdCopy(ctx.weightsGPU, weights.data(), ctx.num_parameters * sizeof(float));
		vkw::CmdBindPipeline(ctx.forwardPipeline);
		NeuralSdfConstants constants;
		constants.width = ctx.width;
		constants.height = ctx.height;
		constants.numLayers = ctx.numLayers;
		constants.layerSize = ctx.layerSize;
		constants.weightsRID = ctx.weightsGPU.RID();
		constants.outputImageRID = ctx.outputImage.RID();
		vkw::CmdPushConstants(&ctx, sizeof(constants));
		vkw::CmdDispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});
		vkw::CmdBarrier();
		vkw::EndCommandBuffer();
		vkw::WaitQueue(vkw::Queue::Compute);

		// vkw::ReadBuffer(ctx.outputImage, pixels.data(), ctx.width * ctx.height * sizeof(Pixel));
		std::vector<unsigned char> image(ctx.width * ctx.height * 4);
		Pixel* mappedMemory = (Pixel*)vkw::MapBuffer(ctx.outputImage);
		for (int i = 0; i < ctx.width * ctx.height; i++) {
			image.push_back(255.0f * mappedMemory[i].r);
			image.push_back(255.0f * mappedMemory[i].g);
			image.push_back(255.0f * mappedMemory[i].b);
			image.push_back(255.0f);
		}
		vkw::UnmapBuffer(ctx.outputImage);
		FileManager::SaveBMP(info->outputPath.c_str(), (const uint32_t*)image.data(), ctx.width, ctx.height);
	}

void NeuralSdfApplication::MainLoop() {
	}

void NeuralSdfApplication::Finish() {
		ctx = {};
		weights.clear();
		vkw::Destroy();
	}
