#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "FileManager.hpp"

#include "HelloTriangle.hpp"
#include "../TestCommon.hpp"
#include "Timer.hpp"

#include "Window.hpp"

using namespace Lmath;

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;

	int width, height;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	vkw::Buffer outputImage;
	vkw::Buffer vertexBuffer;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;
}

struct Vertex {
    vec2 pos;
    vec3 color;
};

const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

void Context::CreateShaders() {
	pipeline = vkw::CreatePipeline({
        .point = vkw::PipelinePoint::Graphics,
        .stages = {
            {.stage = vkw::ShaderStage::Vertex, .path = "tests/HelloTriangle/HelloTriangle.vert"},
            {.stage = vkw::ShaderStage::Fragment, .path = "tests/HelloTriangle/HelloTriangle.frag"},
        },
        .name = "Opaque Pipeline",
        // .vertexAttributes = {vkw::Format::RGB32_sfloat, vkw::Format::RGB32_sfloat, vkw::Format::RGBA32_sfloat, vkw::Format::RG32_sfloat},
		// pos2 + color3
        .vertexAttributes = {vkw::Format::RG32_sfloat, vkw::Format::RGB32_sfloat},
        // .colorFormats = {ctx.albedo.format, ctx.normal.format, ctx.material.format, ctx.emission.format},
        .useDepth = false,
        // .depthFormat = {ctx.depth.format}
    });
}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.outputImage = vkw::CreateBuffer(width * height * sizeof(Pixel), vkw::BufferUsage::Storage, vkw::Memory::CPU, "Output Image");
	ctx.vertexBuffer = vkw::CreateBuffer(vertices.size() * sizeof(Vertex), vkw::BufferUsage::Vertex, vkw::Memory::GPU, "Vertex Buffer");
}

void HelloTriangleApplication::run(HelloTriangleInfo* pHelloTriangleInfo) {
	info = pHelloTriangleInfo;
	Setup();
	Create();
	Draw();
	Finish();
}

void HelloTriangleApplication::Setup() {
	ctx.width = info->width; 
	ctx.height = info->height;
}

void HelloTriangleApplication::Create() {
	Window::Create();
	vkw::Init(Window::GetGLFWwindow(), Window::GetWidth(), Window::GetHeight());
	ctx.CreateImages(ctx.width, ctx.height);
	ctx.CreateShaders();
}


void HelloTriangleApplication::Draw() {
	Timer timer;
	HelloTriangleConstants constants{};
	constants.width = ctx.width;
	constants.height = ctx.height;
	constants.outputImageRID = ctx.outputImage.RID();

	vkw::BeginCommandBuffer(vkw::Queue::Graphics);
	// vkw::CmdBeginRendering(attachs, ctx.depth);
	vkw::CmdBindPipeline(ctx.pipeline);
	vkw::CmdPushConstants(&constants, sizeof(constants));

	vkw::CmdDispatch({(uint32_t)ceil(ctx.width / float(WORKGROUP_SIZE)), (uint32_t)ceil(ctx.height / float(WORKGROUP_SIZE)), 1});

	timer.Start();
	vkw::EndCommandBuffer();
	vkw::WaitQueue(vkw::Queue::Compute);
	printf("Compute time: %fs\n", timer.Elapsed());
	timer.Start();
	saveBuffer("HelloTriangle.bmp", &ctx.outputImage, ctx.width, ctx.height);
	printf("Save time: %fs\n", timer.Elapsed());
}

void HelloTriangleApplication::Finish() {
	ctx = {};
	vkw::Destroy();
}
