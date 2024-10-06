#include "Pch.hpp"

#include "VulkanBase.hpp"

#include "FileManager.hpp"
namespace ImageOptimizer {

struct Context {
    vkw::Pipeline computePipeline;

    std::unordered_map<std::string, int> shaderVersions;

    // vkw::Image compose;


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
	ctx.albedo = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA8_unorm,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled,
		.name = "Albedo Attachment"
	});
	ctx.debug = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA8_unorm,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled,
		.name = "Debug Attachment"
	});
	ctx.normal = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA32_sfloat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled,
		.name = "Normal Attachment"
	});
	ctx.material = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA8_unorm,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled,
		.name = "Material Attachment"
	});
	ctx.emission = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA8_unorm,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled,
		.name = "Emission Attachment"
	});
	ctx.lightA = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA32_sfloat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled | vkw::ImageUsage::Storage,
		.name = "Light Attachment A"
	});
	ctx.lightB = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA32_sfloat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled | vkw::ImageUsage::Storage,
		.name = "Light Attachment B"
	});
	ctx.lightHistory = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA32_sfloat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled | vkw::ImageUsage::Storage,
		.name = "Light History Attachment"
	});
	ctx.depth = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::D32_sfloat,
		.usage = vkw::ImageUsage::DepthAttachment | vkw::ImageUsage::Sampled,
		.name = "Depth Attachment"
	});
	ctx.compose = vkw::CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::BGRA8_unorm,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::Sampled,
		.name = "Compose Attachment"
	});
	ctx.luminanceHistogram = vkw::CreateBuffer(sizeof(float) * 256, vkw::BufferUsage::Storage, vkw::Memory::GPU, "Luminance Histogram");
	ctx.luminanceAverage = vkw::CreateBuffer(sizeof(float), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferSrc, vkw::Memory::GPU | vkw::Memory::CPU, "Luminance Average");
}


void Destroy() {
    ctx = {};
}
};