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


void Destroy() {
    ctx = {};
}
};