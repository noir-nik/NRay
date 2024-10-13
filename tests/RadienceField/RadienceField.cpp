#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "RadienceField.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

#include "Lmath.hpp"
using namespace Lmath;

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;

	int width, height;
	const char *gridPath;
	int gridSize;
	int numCells;
	std::vector<Cell> grid;
	
	float4x4 viewMat;
	float4x4 worldViewInv;
	float4x4 worldViewProjInv;

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
			// {.stage = vkw::ShaderStage::Compute, .path = "tests/RadienceField/RadienceField.slang"},
			{.stage = vkw::ShaderStage::Compute, .path = "tests/RadienceField/RadienceField.comp"},
		},
		.name = "Slang Test",
	});
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.BufferGrid = vkw::CreateBuffer(ctx.numCells * sizeof(Cell), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::GPU, "Grid");
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

	int k = 0;
	float3 pos = float3(0.0, 0.0, 1.3);
	ctx.viewMat = lookAt(pos, float3(0.0, 0.0, 0.0), float3(0.0, 1.0, 0.0)) * rotate4x4Y(-float(360.0 / 7 * k) * DEG_TO_RAD) * translate4x4(float3(-0.5, -0.5, -0.5));
	ctx.worldViewInv = inverse4x4(ctx.viewMat);
	ctx.worldViewProjInv = inverse4x4(perspectiveMatrix(45, 1, 0.1, 100));
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

	constants.worldViewInv = ctx.worldViewInv;
	constants.worldViewProjInv = ctx.worldViewProjInv;

	vkw::BeginCommandBuffer(vkw::Queue::Compute);
	// Prepare BufferGT and BufferOpt
	vkw::CmdCopy(ctx.BufferGrid, ctx.grid.data(), ctx.numCells * sizeof(Cell));
	vkw::CmdBarrier();

	vkw::CmdBindPipeline(ctx.pipeline);
	vkw::CmdPushConstants(&constants, sizeof(constants));

	vkw::CmdDispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});

	timer.Start();
	vkw::EndCommandBuffer();
	vkw::WaitQueue(vkw::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("RadienceField.bmp", &ctx.outputImage, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void RadienceFieldApplication::Finish() {
	ctx = {};
	vkw::Destroy();
}
