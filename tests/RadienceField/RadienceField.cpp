#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "RadienceField.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;

	int width, height;
	const char *gridPath;
	int gridSize;
	int numCells;

	std::vector<Cell> grid;

	vkw::Buffer BufferGrid;
	vkw::Buffer outputImage;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;
}
void Context::CreateShaders() {
	pipeline = vkw::CreatePipeline({
		.point = vkw::PipelinePoint::Compute,
		.stages = {
			{.stage = vkw::ShaderStage::Compute, .path = "tests/RadienceField/RadienceField.slang"},
			// {.stage = vkw::ShaderStage::Compute, .path = "tests/RadienceField/RadienceField.comp"},
		},
		.name = "Slang Test",
	});
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.BufferGrid = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::GPU, "Grid");
	ctx.outputImage = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage, vkw::Memory::CPU, "Output Image");
}

void RadienceFieldApplication::run(RadienceFieldInfo* pRadienceFieldInfo) {
	info = pRadienceFieldInfo;
	Setup();
	Create();
	Compute();
	Finish();
}

void RadienceFieldApplication::Setup() {
	ctx.width = info->width; 
	ctx.height = info->height;
	ctx.gridPath = info->gridPath;
	ctx.gridSize = info->gridSize;
	ctx.numCells = ctx.gridSize * ctx.gridSize * ctx.gridSize;

	// Read binary grid
	std::ifstream fin(ctx.gridPath, std::ios::in | std::ios::binary);
	ASSERT(fin.is_open(), "Failed to open grid file");
	ctx.grid.reserve(ctx.numCells);
	fin.read((char*)ctx.grid.data(), ctx.numCells * sizeof(Cell));
	ASSERT(!fin.fail(), "Failed to read grid file");
	fin.close();
}

void RadienceFieldApplication::Create() {
	vkw::Init();
	ctx.CreateImages(ctx.width, ctx.height);
	ctx.CreateShaders();
}

void RadienceFieldApplication::CmdOptimizationStep(){

}

void RadienceFieldApplication::Compute() {
	Timer timer;
	RadienceFieldConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.outputImageRID = ctx.outputImage.RID();
	constants.gridRID = ctx.BufferGrid.RID();
	constants.gridSize = ctx.gridSize;

	vkw::BeginCommandBuffer(vkw::Queue::Compute);
	// Prepare BufferGT and BufferOpt
	vkw::CmdCopy(ctx.BufferGT, imageUV.data(), ctx.width * ctx.height * sizeof(Pixel));
	vkw::CmdBarrier(ctx.imageCPU, vkw::Layout::General);
	vkw::CmdClearColorImage(ctx.imageCPU, {0.0f, 0.0f, 0.5f, 0.0f});
	vkw::CmdBarrier(ctx.imageCPU, vkw::Layout::TransferSrc);
	vkw::CmdCopy(ctx.BufferOpt, ctx.imageCPU);
	vkw::CmdBarrier();

	vkw::CmdBindPipeline(ctx.pipeline);
	vkw::CmdPushConstants(&constants, sizeof(constants));

	vkw::CmdDispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});
	vkw::CmdBarrier();
	vkw::CmdCopy(ctx.bufferCPU, ctx.BufferOpt, ctx.width * ctx.height * sizeof(Pixel));

	timer.Start();
	vkw::EndCommandBuffer();
	vkw::WaitQueue(vkw::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("imageOpt.bmp", &ctx.bufferCPU, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void RadienceFieldApplication::Finish() {
	ctx = {};
	vkw::Destroy();
}
