#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"

#include "FileManager.hpp"
namespace ImageOptimizer {

struct Context {
    vkw::Pipeline computePipeline;

    std::unordered_map<std::string, int> shaderVersions;

    // vkw::Image compose;

	vkw::Buffer luminanceHistogram;
	vkw::Buffer luminanceAverage;
};
Context ctx;

void CreatePipeline(vkw::Pipeline& pipeline, const vkw::PipelineDesc& desc) {
	bool should_update = false;
	for (auto& stage : desc.stages) {
		auto path = "source/Shaders/" + stage.path.string();
		auto it = ctx.shaderVersions.find(path);
		auto version = FileManager::GetFileVersion(path);
		// TODO: Check for -1 (file not found)

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
    CreatePipeline(ctx.computePipeline, {
        .point = vkw::PipelinePoint::Compute,
        .stages = {
            {.stage = vkw::ShaderStage::Compute, .path = "imageOptimization.comp"},
        },
        .name = "Image Optimization Pipeline",
    });
}

void CreateImages(uint32_t width, uint32_t height) {
	// ctx.lightA = vkw::CreateImage({
	// 	.width = width,
	// 	.height = height,
	// 	.format = vkw::Format::RGBA32_sfloat,
	// 	.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled | vkw::ImageUsage::Storage,
	// 	.name = "Light Attachment A"
	// });
	ctx.luminanceHistogram = vkw::CreateBuffer(sizeof(float) * 256, vkw::BufferUsage::Storage, vkw::Memory::GPU, "Luminance Histogram");
	ctx.luminanceAverage = vkw::CreateBuffer(sizeof(float), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferSrc, vkw::Memory::GPU | vkw::Memory::CPU, "Luminance Average");
}

void forwardPass() {
	vkw::CmdBindPipeline(ctx.computePipeline);
	imageOptConstants constants;
	constants.factor = 1;

	vkw::CmdPushConstants(&constants, sizeof(imageOptConstants));

}

void Destroy() {
    ctx = {};
}
};