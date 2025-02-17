#ifdef USE_MODULES
import lmath;
import vulkan_backend;
#else
#include "lmath.hpp"
#include <vulkan_backend/core.hpp>
#endif
#include "Pch.hpp"
#include "Bindless.h"
#include "FileManager.hpp"

#include "RadienceField.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

using namespace lmath;

using Pixel = vec4;
namespace {
struct Context {
	vb::Pipeline pipeline;

	int width, height;
	char const *gridPath;
	int gridSize;
	int numCells;
	std::vector<Cell> grid;
	
	float4x4 viewMat;
	float4x4 worldViewInv;
	float4x4 worldViewProjInv;

	vb::Buffer BufferGrid;
	vb::Buffer outputImage;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;
}
void Context::CreateShaders() {
	pipeline = vb::CreatePipeline({
		.point = vb::PipelinePoint::Compute,
		.stages = {
			// {.stage = vb::ShaderStage::Compute, .path = "tests/RadienceField/RadienceField.slang"},
			{.stage = vb::ShaderStage::Compute, .path = "tests/RadienceField/RadienceField.comp"},
		},
		.name = "Slang Test",
	});
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.BufferGrid = vb::CreateBuffer(ctx.numCells * sizeof(Cell), vb::BufferUsage::Storage | vb::BufferUsage::TransferDst, vb::Memory::GPU, "Grid");
	ctx.outputImage = vb::CreateBuffer(width * height * sizeof(Pixel), vb::BufferUsage::Storage, vb::Memory::CPU, "Output Image");
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
	ctx.worldViewProjInv = inverse4x4(perspective(45, 1, 0.1, 100));
}

void RadienceFieldApplication::Create() {
	vb::Init();
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

	constants.bBoxMin = {0.0, 0.0, 0.0};
	constants.bBoxMax = {1.0, 1.0, 1.0};

	constants.worldViewInv = ctx.worldViewInv;
	constants.worldViewProjInv = ctx.worldViewProjInv;

	auto cmd = vb::GetCommandBuffer(vb::Queue::Compute);
	cmd.BeginCommandBuffer();
	// Prepare BufferGT and BufferOpt
	cmd.Copy(ctx.BufferGrid, ctx.grid.data(), ctx.numCells * sizeof(Cell));
	cmd.Barrier();

	cmd.BindPipeline(ctx.pipeline);
	cmd.PushConstants(&constants, sizeof(constants));

	cmd.Dispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});

	timer.Start();
	cmd.EndCommandBuffer();
	vb::WaitQueue(vb::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("RadienceField.bmp", &ctx.outputImage, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void RadienceFieldApplication::Finish() {
	ctx = {};
	vb::Destroy();
}
