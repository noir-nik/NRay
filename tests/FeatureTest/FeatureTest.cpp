#ifdef USE_MODULES
module;
#endif

#include "Phong.h"
#include "Opaque.h"
#include "GpuTypes.h"

#ifdef USE_MODULES
module FeatureTest;
import glfw;
import imgui;
import Window;
import lmath;
import vulkan_backend;
import Component;
import std.compat;
import std;
import Log;
import Editor;
import Entity;
import Project;
import Objects;
import GLTFLoader;
import entt;
import UI;
import Runtime;
import Camera;
import Types;
import Structs;
import Materials;
#else
#include "lmath.hpp"
#include <vulkan_backend/core.hpp>
#include "Window.cppm"
#include "Log.cppm"
#include "Editor.cppm"
#include "FeatureTest.cppm"
#include "GLTFLoader.cppm"
#include "Project.cppm"
#include "UI.cppm"
#include "Types.cppm"
#include "Structs.cppm"
#include "Materials.cppm"
#include "Runtime.cppm"

#include <cstdint>
#include <cstdio>
#include <vector>

#endif

#define GSLS_OPTIONS "-DGLSL -Isource/Shaders/include"
#define BIN_PATH "bin"

using namespace lmath;
using namespace Engine;

namespace {

struct AppContext : Structs::NoCopyNoMove {

	vb::Pipeline pipeline;
	vb::Pipeline glTFPipeline;
	vb::Pipeline texPipeline;
	vb::Pipeline opaquePipeline;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	vb::SampleCount sample_count = vb::SampleCount::_4;
	// vb::SampleCount sampleCount = vb::SampleCount::_1;
	// vb::Format renderFormat = vb::Format::RGBA16Sfloat;
	vb::Format renderFormat = vb::Format::RGBA8Unorm;
	
	vb::Format depthFormat = vb::Format::D32Sfloat;

	// vb::Buffer outputImage;
	vb::Buffer cubeVertexBuffer;

	vb::Instance instance;
	vb::Device device;

	vb::Queue queue;
	
	Entity mainWindow;

	Entity phongMaterial;
	Entity phongLight;

	vb::Image errorImage;
	// vb::Image blackImage;
	// vb::Image grayImage;
	// vb::Image whiteImage;

	std::unordered_map<Window*, vb::Swapchain> swapchains;

	Component::Material defaultMaterial;
	Materials materials;

	// Runtime::Context runtimeContext;

	Project project;

	char const* gltfPath;

	// std::vector<Entity> meshes;
	void Setup();
	void CreateShaders();
	void CreateDefaultResources();
	void CreateWindowResources(Entity window);
	void UploadBuffers();
	void DrawWindow(Entity window);
	void viewport();
	void RenderUI();
	void DrawImgui(Entity window);
	void DrawViewport(vb::Command& cmd, Camera const& camera);
	void RecreateFrameResources(Window* window);

	void test();
};
static AppContext* ctx;

struct WindowImageResource : Structs::NoCopyNoMove {
	vb::Image colorImage;
	vb::Image resolveImage;
	vb::Image depthImage;
	vb::Image uiColorImage;
	vb::Image uiResolveImage;

};


// static Runtime::Camera camera(vec3(0.0f, 0.0f, 30.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

struct VertexDebug {
	vec3 pos;
	vec3 color;
};

void FramebufferCallback(Window* window, int width, int height);
void RefreshCallback(Window* window);
void MouseButtonCallback(Window* window, int button, int action, int mods);
void KeyCallback(Window* window, int key, int scancode, int action, int mods);
void CursorPosCallback(Window *window, double xpos, double ypos);
void ScrollCallback(Window *window, double xoffset, double yoffset);
void IconifyCallback(Window* window, int iconified);
[[maybe_unused]] void PrintMatrix(const float4x4& m, char const* name = nullptr);
// const std::vector<Vertex> vertices = {
// 	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
// 	{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
// 	{{0.5f, 0.5f, 0.0f},  {0.0f, 1.0f, 0.0f}},

// 	{{0.0f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
// 	{{-0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}},
// 	{{0.5f, 0.5f, 1.0f},  {0.0f, 1.0f, 0.0f}},

// };

// Cube
std::vector<VertexDebug> const vertices = {
	{{-0.5f, -0.5f, -0.5f}, { 0.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f, 0.0f}},
	{{0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{-0.5f, -0.5f, -0.5f}, { 0.0f, 0.0f, 0.0f}},
	{{-0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{0.5f,  0.5f,  0.5f}, { 1.0f, 1.0f, 0.0f}},
	{{0.5f,  0.5f,  0.5f}, { 1.0f, 1.0f, 0.0f}},
	{{-0.5f,  0.5f,  0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{-0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f, 0.0f}},
	{{-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{-0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f, 0.0f}},
	{{0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, { 1.0f, 1.0f, 0.0f}},
	{{0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{-0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f, 0.0f}},
	{{-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f}},
	{{0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f, 0.0f}},
	{{0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f,  0.5f}, { 0.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},
};

void AppContext::CreateShaders() {
	auto& resource = mainWindow.Get<WindowImageResource>();
	bool constexpr skip_recompilation = false; 

	char compile_options[128];
	std::snprintf(compile_options, sizeof(compile_options), 
		"-DBINDING_TEXTURE=%u -DBINDING_BUFFER=%u %s", 
		device.GetBinding(vb::DescriptorType::CombinedImageSampler),
		device.GetBinding(vb::DescriptorType::StorageBuffer),
		GSLS_OPTIONS);

/* 	pipeline = device.CreatePipeline({
		.point = vb::PipelinePoint::Graphics,
		.stages = {{
			{.stage = vb::ShaderStage::Vertex, .filename = "tests/FeatureTest/FeatureTest.vert"},
			{.stage = vb::ShaderStage::Fragment, .filename = "tests/FeatureTest/FeatureTest.frag"},
		}},
		.name = "Feature pipeline",
		// pos2 + color3
		.vertexAttributes = {vb::Format::RGB32Sfloat, vb::Format::RGB32Sfloat},
		.colorFormats = {{renderFormat}},
		.useDepth = true,
		.depthFormat = resource.depthImage.format,
		.samples = sampleCount,
		.lineTopology = false
	});

	glTFPipeline = device.CreatePipeline({
		.point = vb::PipelinePoint::Graphics,
		.stages = {{
			{.stage = vb::ShaderStage::Vertex, .filename = "source/Shaders/Phong.vert"},
			{.stage = vb::ShaderStage::Fragment, .filename = "source/Shaders/Phong.frag"},
		}},
		.name = "Phong pipeline",
		.vertexAttributes = {
			vb::Format::RGB32Sfloat,  // vec3 position;
			vb::Format::RGB32Sfloat,  // vec3 normal;
			vb::Format::RG32Sfloat,   // vec2 uv;
			vb::Format::RGBA32Sfloat, // vec4 color;
			vb::Format::RGBA32Sfloat  // vec4 tangent;
		},
		.colorFormats = {{renderFormat}},
		.useDepth = true,
		.depthFormat = resource.depthImage.format,
		.samples = sampleCount,
		.lineTopology = false
	});
 */
	texPipeline = device.CreatePipeline({
		.point = vb::PipelinePoint::Graphics,
		.stages = {{
			{
				.stage = vb::ShaderStage::Vertex, 
				.source = {"source/Shaders/Quad.vert"},
				.out_path = BIN_PATH,
				.compile_options = compile_options,
				.allow_skip_compilation = skip_recompilation,
			},
			{
				.stage = vb::ShaderStage::Fragment,
				.source = {"source/Shaders/TexImage.frag"},
				.out_path = BIN_PATH,
				.compile_options = compile_options,
				.allow_skip_compilation = skip_recompilation,
			}
		}},
		.color_formats = {{renderFormat}},
		.use_depth = true,
		.depth_format = resource.depthImage.GetFormat(),
		.samples = sample_count,
		.line_topology = false,
		.name = "Tex pipeline",
	});

	opaquePipeline = device.CreatePipeline({
		.point = vb::PipelinePoint::Graphics,
		.stages = {{
			{
				.stage = vb::ShaderStage::Vertex, 
				.source = {"source/Shaders/Opaque.vert"},
				.out_path = BIN_PATH,
				.compile_options = compile_options,
				.allow_skip_compilation = skip_recompilation,
			},
			{
				.stage = vb::ShaderStage::Fragment,
				.source = {"source/Shaders/Opaque.frag"},
				.out_path = BIN_PATH,
				.compile_options = compile_options,
				.allow_skip_compilation = skip_recompilation,
			}
		}},
		.vertexAttributes = {{
			vb::Format::RGB32Sfloat,  // vec3 position;
			vb::Format::RGB32Sfloat,  // vec3 normal;
			vb::Format::RG32Sfloat,   // vec2 uv;
			vb::Format::RGBA32Sfloat, // vec4 color;
			vb::Format::RGBA32Sfloat  // vec4 tangent;
		}},
		.color_formats = {{renderFormat}},
		.use_depth = true,
		.depth_format = resource.depthImage.GetFormat(),
		.samples = sample_count,
		.line_topology = false,
		.name = "Opaque pipeline",
	});

}

void AppContext::CreateDefaultResources() {

	// Error Texture
	constexpr int errorImageSize = 16;
	
	errorImage = device.CreateImage({
		.extent = {errorImageSize, errorImageSize},
		.format = vb::Format::RGBA8Unorm,
		.usage = vb::ImageUsage::Sampled | vb::ImageUsage::TransferDst,
		.sampler = {vb::Filter::Nearest, vb::Filter::Nearest, vb::MipmapMode::Nearest},
		.name = "Error Texture"
	});

	LOG_INFO("Created null image {}", errorImage.GetResourceID());
	DEBUG_ASSERT(errorImage.GetResourceID() == 0, "Error image must be created with RID 0");

	u32 magenta = packRGBA({1, 0, 1, 1});
	u32 black = packRGBA({0, 0, 0, 1});
	std::array<u32, errorImageSize * errorImageSize> errorImagePixels;
	for (int x = 0; x < errorImageSize; ++x) {
		for (int y = 0; y < errorImageSize; ++y) {
			errorImagePixels[x * errorImageSize + y] = ((x % 2) ^ (y % 2)) ? magenta : black;
		}
	}

	// Default Material
	defaultMaterial = {
		.name = "Default material",
		.gpuMaterial = {
			.baseColorFactor  = {1.0f, 1.0f, 1.0f, 1.0f},
			.emissiveFactor   = {1.0f, 1.0f, 1.0f},
			// .emissiveColor    = {1.0f, 1.0f, 1.0f},
			// .emissiveStrength = 0.0f,
			.metallicFactor   = 0.0f,
			.roughnessFactor  = 0.5f,
		},
		.deviceMaterialID = materials.CreateSlot()
	};

	DEBUG_ASSERT(defaultMaterial.deviceMaterialID == 0, "Default material ID must be 0");

	auto cmd = queue.GetCommandBuffer();
	cmd.Begin();
	// Error Image
	cmd.Barrier(errorImage, {vb::ImageLayout::TransferDst});
	cmd.Copy(errorImage, &errorImagePixels[0], errorImagePixels.size() * sizeof(errorImagePixels[0]));
	// Default Material
	cmd.Copy(materials.GetBuffer(defaultMaterial.deviceMaterialID), &defaultMaterial.gpuMaterial, sizeof(GpuTypes::Material), sizeof(GpuTypes::Material) * defaultMaterial.deviceMaterialID);
	cmd.End();
	cmd.QueueSubmit();
}

void AppContext::CreateWindowResources(Entity window) {
	using namespace std::literals;
	Window& windowHandle = window.Get<Window>();
	uint2 size = uint2(windowHandle.GetSize());
	swapchains[&windowHandle] = device.CreateSwapchain({
		.window = windowHandle.GetGLFWwindow(), 
		.extent = {size.x, size.y},
		.queue = queue,
	});
	swapchains[&windowHandle].CreateImGui(sample_count);
	windowHandle.AddFramebufferSizeCallback(FramebufferCallback);
	windowHandle.AddMouseButtonCallback(MouseButtonCallback);
	windowHandle.AddWindowRefreshCallback(RefreshCallback);
	windowHandle.AddKeyCallback(KeyCallback);
	windowHandle.AddCursorPosCallback(CursorPosCallback);
	windowHandle.AddScrollCallback(ScrollCallback);
	windowHandle.AddWindowIconifyCallback(IconifyCallback);

	uint2 sizeLimits = uint2(windowHandle.GetSizeLimits().zw());
	vb::Extent3D maxSize = {sizeLimits.x, sizeLimits.y};

	auto& resource = window.Get<WindowImageResource>();

	resource.depthImage = device.CreateImage({
        .extent = maxSize,
        .format = depthFormat,
        .usage = vb::ImageUsage::DepthStencilAttachment | vb::ImageUsage::TransientAttachment,
		.samples = sample_count,
        .name = "Depth Attachment"
    });

	resource.resolveImage = device.CreateImage({
		.extent = maxSize,
		.format = renderFormat,
		.usage = vb::ImageUsage::ColorAttachment | vb::ImageUsage::TransferSrc,
		.name = windowHandle.GetName() + "_resolve"s,
	});
	
	resource.colorImage = device.CreateImage({
		.extent = maxSize,
		.format = renderFormat,
		.usage = vb::ImageUsage::ColorAttachment | vb::ImageUsage::TransientAttachment,
		.samples = sample_count,
		.name = windowHandle.GetName() + "_color"s,
	});
	
	resource.uiResolveImage = device.CreateImage({
		.extent = maxSize,
		.format = renderFormat,
		.usage = vb::ImageUsage::ColorAttachment | vb::ImageUsage::TransferSrc,
		.name = windowHandle.GetName() + "_ui_resolve"s,
	});
	
	resource.uiColorImage = device.CreateImage({
		.extent = maxSize,
		.format = renderFormat,
		.usage = vb::ImageUsage::ColorAttachment | vb::ImageUsage::TransientAttachment,
		.samples = sample_count,
		.name = windowHandle.GetName() + "_ui_color"s,
	});
}

void AppContext::UploadBuffers() {
	auto& cmd = queue.GetCommandBuffer();
	cmd.Begin();
	cmd.Copy(cubeVertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(VertexDebug));
	cmd.Barrier();
	cmd.End();
	cmd.QueueSubmit();

	glTF::Loader loader;
	auto res = loader.Load({gltfPath, project.GetSceneGraph(), materials, device, queue, errorImage.GetResourceID()});
	ASSERT(res, "Failed to load gltf file");

	auto& sceneGraph = project.GetSceneGraph();
	sceneGraph.DebugPrint();
}

void AppContext::DrawViewport(vb::Command& cmd, Camera const& camera) {

	// cmd.Barrier(resource.renderImage, {vb::ImageLayout::TransferDst});
	// cmd.ClearColorImage(resource.renderImage, {0.7f, 0.0f, 0.4f, 1.0f});

/*  // Phong
	Phong::PhongConstants constants{};
	constants.viewProj = camera.proj * (camera.view | affineInverse);
	constants.light = phongLight.Get<Phong::PhongLight>();
	constants.material = phongMaterial.Get<Phong::PhongMaterial>();
	constants.cameraPosition = camera.getPosition();
	cmd.BindPipeline(glTFPipeline);
	auto registry = project.GetSceneGraph().registry;
	for (auto& meshNode : registry->view<Component::Mesh>()) {
		constants.model = registry->get<Component::Transform>(meshNode).global;
		cmd.PushConstants(glTFPipeline, &constants, sizeof(constants));
		Component::Mesh& mesh = registry->get<Component::Mesh>(meshNode);
		cmd.DrawMesh(mesh.vertexBuffer, mesh.indexBuffer, mesh.indexBuffer.size / sizeof(uint32_t));
	}
 */

/*  // Cube
	FeatureTestConstants debugConstants{
		.view = camera.view | affineInverse,
		.proj = camera.proj,
		.viewProj = camera.proj * (camera.view | affineInverse),
	};
	cmd.BindPipeline(pipeline);
	cmd.PushConstants(&debugConstants, sizeof(debugConstants));
	cmd.BindVertexBuffer(cubeVertexBuffer);
	cmd.Draw(vertices.size(), 1, 0, 0);
 */

/* 	// Quad
	cmd.BindVertexBuffer(cubeVertexBuffer);
	cmd.BindPipeline(texPipeline);
	cmd.Draw(6, 1, 0, 0);
 */


 	Opaque::OpaqueConstants constants{};
	constants.viewProj = camera.proj * (camera.view | affineInverse);
	constants.light = {
		.position = vec3(2.0, 2.0, 2.0),
		.color = vec3(0.8, 0.8, 0.8),
		.intensity = 8.0,
		.ambientColor = vec3(0.9, 0.9, 0.9),
		.ambientIntensity = 0.03
	};
	constants.cameraPosition = camera.getPosition();

	cmd.BindPipeline(opaquePipeline);
	auto registry = project.GetSceneGraph().registry;
	for (auto& meshNode : registry->view<Component::Mesh*>()) {
		auto mesh = registry->get<Component::Mesh*>(meshNode);
		constants.model = registry->get<Component::Transform>(meshNode).global;
		cmd.BindVertexBuffer(mesh->vertexBuffer);
		cmd.BindIndexBuffer(mesh->indexBuffer);
		for (auto& p : mesh->primitives) {
			// LOG_INFO("Draw primitive {} {}", p.material->deviceMaterialID, mesh->name);
			auto materialID = p.material ? p.material->deviceMaterialID : defaultMaterial.deviceMaterialID;
			constants.materialBufferRID = materials.GetBuffer(materialID).GetResourceID();
			constants.materialOffset = materials.GetOffset(materialID) / sizeof(GpuTypes::Material);
			cmd.PushConstants(opaquePipeline, &constants, sizeof(constants));
			cmd.DrawIndexed(p.indexCount, p.instanceCount, p.firstIndex, p.vertexOffset, p.firstInstance);
			// cmd.DrawMesh(mesh.vertexBuffer, mesh.indexBuffer, mesh.indexBuffer.size / sizeof(uint32_t));
		}
	}
	
}

void AppContext::DrawWindow(Entity window) {
	auto& windowHandle = window.Get<Window>();
	auto& windowData   = window.Get<Editor::WindowData>();
	auto& resource     = window.Get<WindowImageResource>();
	auto& swapchain    = swapchains[&windowHandle];
	auto& cmd          = swapchain.GetCommandBuffer();

	windowHandle.SetUIContextCurrent();

	Editor::BeginFrame();
	Editor::Draw({&ctx->project.GetSceneGraph(), &windowData});
	auto uiDrawData = Editor::EndFrame();

	// ImGuiIO& io = ImGui::GetIO();
	// if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
	// 	ImGui::UpdatePlatformWindows();
	// 	ImGui::RenderPlatformWindowsDefault();
	// }

	cmd.Begin();
	if (!swapchain.AcquireImage()) {
		LOG_WARN("AcquireImage: Swapchain dirty");
		return;
	}
	auto& targetImage = swapchain.GetCurrentImage();
	

/* 	
	std::span<Runtime::Panel> panels;
	if (windowData.main) {
		panels = windowData.tabPanels;
	} else{
		panels = windowData.panels;
	}

	for (auto& panel : panels) {
		if (std::holds_alternative<Runtime::Viewport>(panel)) {
			auto& v = std::get<Runtime::Viewport>(panel);
			auto &viewport = v.viewport;
			ivec4 flipped (viewport.x, viewport.y + viewport.w, viewport.z, -viewport.w);
			// ivec4 flipped = viewport;
			// LOG_INFO("Viewport: {}, {}, {}, {}", flipped.x, flipped.y, flipped.z, flipped.w);
			cmd.SetViewport(flipped);
			DrawViewport(cmd, v.camera);
		}
	} */

	vb::Rect2D fullWindowRect({0, 0}, {static_cast<u32>(windowHandle.GetWidth()), static_cast<u32>(windowHandle.GetHeight())});
	cmd.SetScissor(fullWindowRect);

	static ivec4 viewport_A;
	cmd.Barrier(resource.colorImage, {vb::ImageLayout::ColorAttachment});
	cmd.Barrier(resource.resolveImage, {vb::ImageLayout::ColorAttachment});
	cmd.Barrier(resource.depthImage, {vb::ImageLayout::DepthAttachment});
	// Viewports
	if (!windowData.viewportsToRender.empty()) {
		vb::ClearColorValue clear_color = {{0.1f, 0.1f, 0.1f, 1.0f}};
		if (sample_count == vb::SampleCount::_1) {
			cmd.BeginRendering({
				.colorAttachs = {{{
					.colorImage = resource.resolveImage, 
					.clearValue = clear_color
				}}},
				.depth        = {resource.depthImage},
				.renderArea   = fullWindowRect
			});
		} else {
			cmd.BeginRendering({
				.colorAttachs = {{{
					.colorImage   = resource.colorImage, 
					.resolveImage = resource.resolveImage, 
					.clearValue   = clear_color
				}}}, 
				.depth        = {resource.depthImage},
				.renderArea   = fullWindowRect
			});
		};

		for (auto v : windowData.viewportsToRender) {
		// for (auto p : windowData.panels) {
		// 	if (!std::holds_alternative<Editor::Viewport>(p)) continue;
		// 	auto v = &std::get<Editor::Viewport>(p);
			// LOG_INFO("Viewport: {}, {}, {}, {}", v->rect.x, v->rect.y, v->rect.z, v->rect.w);
			ivec4 viewport(v->rect.x, v->rect.y, v->rect.z - v->rect.x, v->rect.w - v->rect.y);
			viewport_A = viewport;
			vb::Viewport flipped (viewport.x, viewport.y + viewport.w, viewport.z, -viewport.w);
			cmd.SetViewport(flipped);
			DrawViewport(cmd, v->camera);
		}
		cmd.EndRendering();
	}

	// Blit viewport to swapchain image
	cmd.Barrier(resource.resolveImage, {vb::ImageLayout::TransferSrc});
	cmd.Barrier(targetImage,           {vb::ImageLayout::TransferDst});

	vb::ImageBlit::Region blitRect = {
		{viewport_A.x, viewport_A.y, 0}, 
		{viewport_A.z + viewport_A.x, viewport_A.w + viewport_A.y, 1}
	};
	cmd.Blit({
		.dst = targetImage,
		.src = resource.resolveImage,
		.regions = {{{blitRect, blitRect}}}
	});

	cmd.Barrier(targetImage, {vb::ImageLayout::ColorAttachment});
	// Render ui to swapchain image
	cmd.BeginRendering({
		.colorAttachs   = {{{
			.colorImage = targetImage,
			.loadOp     = vb::LoadOp::Load
		}}},
		.renderArea     = fullWindowRect,
	});
	cmd.DrawImGui(uiDrawData);
	cmd.EndRendering();
	
	cmd.Barrier(targetImage, {vb::ImageLayout::Present});

	swapchain.SubmitAndPresent();
	if (swapchain.GetDirty()) {
		LOG_WARN("SubmitAndPresent: Swapchain dirty");
		return;
	}

	windowHandle.AddFramesToDraw(-1);
	windowHandle.SetFrameCount((windowHandle.GetFrameCount() + 1) % (1 << 15));
}

void KeyCallback(Window* window, int key, int scancode, int action, int mods) {
	// printf("key: %d, scancode: %d, action: %d, mods: %d\n", key, scancode, action, mods);
	window->AddFramesToDraw(1);
	auto glfwkey = static_cast<GLFW::Key>(key);
	switch (action){
	case GLFW::Press:
		switch (glfwkey) {
		// case GLFW::Key::N: {
		// 	static int windowCount = 1;
		// 	std::string name = "w" + std::to_string(windowCount++);
		// 	auto window = new Window(ctx->width, ctx->height, name.c_str());
		// 	ctx->CreateWindowResources(window);
		// 	break;
		// }

/* 		
		case GLFW::Key::W:
			camera.view = translate4x4({0, 0, -0.02f}) * camera.view;
			break;
		case GLFW::Key::S:
			camera.view = translate4x4({0, 0, 0.02f}) * camera.view;
			break;
		case GLFW::Key::A:
			camera.view = translate4x4({0.02f, 0, 0}) * camera.view;
			break;
		case GLFW::Key::D:
			camera.view = translate4x4({-0.02f, 0, 0}) * camera.view;
			break;
		case GLFW::Key::G: {


				// auto& io = ImGui::GetIO();
				// auto& fonts = io.Fonts;
				// unsigned char* texData;
				// int texWidth, texHeight;
				// fonts->GetTexDataAsAlpha8(&texData, &texWidth, &texHeight);
				// // printf("Font texture size: %dx%d\n", texWidth, texHeight);
				// // printf("Font texture data: %p\n", texData);
				// std::vector<uint32_t> texData4(texWidth * texHeight);
				// for (int i = 0; i < texWidth * texHeight; ++i) {
				// 	uint32_t pixel = 0;
				// 	pixel |= (texData[i] << 24);
				// 	pixel |= (texData[i] << 16);
				// 	pixel |= (texData[i] << 8);
				// 	pixel |= (texData[i] << 0);
				// 	texData4[i] = pixel;
				// }
				// FileManager::SaveBMP("font.bmp", texData4.data(), texWidth, texHeight);
				// break;
		} */



		// case GLFW::Key::J: {
		// 	ImGuiIO& io = ImGui::GetIO();
		// 	const float fontSize = 17.0f;
		// 	// Editor::BeginFrame();
		// 	vb::BeginImGui();
		// 	for (const auto& entry : std::filesystem::directory_iterator("bin")) {
		// 		if (entry.filename().extension() == ".ttf" || entry.filename().extension() == ".otf") {
		// 			auto font = io.Fonts->AddFontFromFileTTF(entry.filename().string().c_str(), fontSize);
		// 			if (font && entry.filename().filename() == "segoeui.ttf") {
		// 				io.FontDefault = font;
		// 			}
		// 			// if (entry.filename().filename() == "segoeui.ttf") {
		// 			// 	auto font = io.Fonts->AddFontFromFileTTF(entry.filename().string().c_str(), fontSize);
		// 			// 	io.FontDefault = font;
		// 			// }
		// 		}
		// 	}
		// 	// Editor::EndFrame();
		// 	break;
		// }
		default:
			break;
		}
		break;
	case GLFW::Release:
		break;
	case GLFW::Repeat:
		switch (key) {
		
		default:
			break;
		}

	}
}


enum Hit {
	Client = 0,
	Left = 1, Top = 2, Right = 4, Bottom = 8,
	TopLeft = 5, TopRight = 6, BottomLeft = 9, BottomRight = 10,
};

[[maybe_unused]] Hit MouseHitTest (Window *window, double xpos, double ypos) {
	// auto& border_thickness = window->GetBorderThickness();
	// l t r b
	lmath::ivec4 border_thickness = { 50, 30, 50, 50 };
	auto size = window->GetSize();
	enum { left = 1, top = 2, right = 4, bottom = 8 };
	int hit = 0;
	if (xpos < border_thickness.x) hit |= left;
	if (ypos < border_thickness.y) hit |= top;
	if (xpos > size.x - border_thickness.z) hit |= right;
	if (ypos > size.y - border_thickness.w) hit |= bottom;

	if (hit & Top && hit & Left) return Hit::TopLeft;
	if (hit & Top && hit & Right) return Hit::TopRight;
	if (hit & Bottom && hit & Left) return Hit::BottomLeft;
	if (hit & Bottom && hit & Right) return Hit::BottomRight;
	if (hit & Left) return Hit::Left;
	if (hit & Top) return Hit::Top;
	if (hit & Right) return Hit::Right;
	if (hit & Bottom) return Hit::Bottom;

	return Hit::Client;
}

void CursorPosCallback (Window *window, double xpos, double ypos) {
	window->AddFramesToDraw(1);
/* 
	const auto& camera_right = camera.getRight();
	const auto& camera_up = camera.getUp();
	const auto& camera_forward = camera.getForward();
	auto& camera_pos = camera.getPosition();



	// Hit hit = MouseHitTest(window, xpos, ypos);
	// switch (hit) {
	// case Hit::Left:        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); break;
	// case Hit::Top:         ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS); break;
	// case Hit::Right:       ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); break;
	// case Hit::Bottom:      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS); break;
	// case Hit::TopLeft:     ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE); break;
	// case Hit::TopRight:    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW); break;
	// case Hit::BottomLeft:  ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW); break;
	// case Hit::BottomRight: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE); break;
	// default: ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow); break;
	// }
	

	if (mouse.buttons[static_cast<int>(GLFW::MouseButton::Right)]) {
		switch (mouse.mods)
		{
		case GLFW::Mod::Alt: {
			auto zoom_factor = camera.zoom_factor * length(camera_pos - camera.focus);
			auto movement = (zoom_factor * mouse.deltaPos.x) * camera_forward;
			camera.view = translate4x4(movement) * camera.view;
			// camera.focus += movement;
			break;
		}
		
		default:
			camera_pos -= camera.focus;
			// camera.view = rotate4x4(camera_up, mouse.deltaPos.x * camera.rotation_factor) * rotate4x4(camera_right, mouse.deltaPos.y * camera.rotation_factor)  * camera.view; // trackball
			camera.view = rotate4x4Y((camera_up.y > 0 ? 1.0f : -1.0f) * mouse.deltaPos.x * camera.rotation_factor) * rotate4x4(camera_right, mouse.deltaPos.y * camera.rotation_factor)  * camera.view;
			camera_pos += camera.focus;

			break;
		}
		// window->AddFramesToDraw(1);
	}
	if (mouse.buttons[GLFW::MouseButton::Middle]) {
		auto move_factor = camera.move_factor * length(camera_pos - camera.focus);
		auto movement = move_factor * (camera_up * -mouse.deltaPos.y + camera_right * mouse.deltaPos.x); 
		// printf("%f %f %f\n", movement.x, movement.y, movement.z);
		// camera.focus += movement;
		camera.view = translate4x4(movement) * camera.view;
		// window->AddFramesToDraw(1);
	}
	if (mouse.buttons[GLFW::MouseButton::Left]) {
		// window->AddFramesToDraw(1);
	}
 */
}
void MouseButtonCallback(Window* window, int button, int action, int mods) {
	if (action != GLFW::Repeat) {
		window->AddFramesToDraw(2);
	}
	switch (button) {
	case GLFW::MouseButton::Left:
		// if (action == GLFW::PRESS) {
			// window->AddFramesToDraw(1);
		// }
		break;
	default:
		break;
	}
}

void ScrollCallback(Window *window, double xoffset, double yoffset){
	window->AddFramesToDraw(1);
}

void FramebufferCallback(Window* window, int width, int height) {
	if (width == 0 || height == 0) {return;}
	// camera.setProj(fov, width, height, zNear, zFar);

	// LOG_INFO("RecreateFrameResources {} callback", window->GetName());	
	ctx->RecreateFrameResources(window);
}

void RefreshCallback(Window* window) {
	auto size = window->GetSize();
	if (size.x == 0 || size.y == 0 || !window->GetAlive()) [[unlikely]] return;
	window->AddFramesToDraw(1);
	ctx->DrawWindow(Entity(&ctx->project.GetRegistry(), static_cast<entt::entity>(window->GetEntityHandle())));
}

void IconifyCallback(Window* window, int iconified) {
	if (iconified) {
		window->SetFramesToDraw(0);
	}
}

void AppContext::RecreateFrameResources(Window* window) {
	if (!window->GetAlive()) { /* LOG_WARN("RecreateFrameResources: Window is dead"); */ return;} // important
	if (window->GetIconified()) {/* LOG_TRACE("RecreateFrameResources: size = 0"); */ return;};

	device.WaitIdle();
	auto swapchain = swapchains[window];
	bool swapChainDirty = swapchain.GetDirty();
	bool framebufferResized = window->GetFramebufferResized();
	// LOG_INFO("RecreateFrameResources {} {} {} {}", window->GetName(), (void*)window->GetGLFWwindow(), swapChainDirty, framebufferResized);
	if (swapChainDirty || framebufferResized) {
		// LOG_INFO("DIRTY FRAME RESOURCES");
		window->UpdateFramebufferSize();
		// window->RecreateSwapchain();
		uint2 size = uint2(window->GetSize());
		swapchain.Recreate(size.x, size.y);
		window->SetFramebufferResized(false);
		window->AddFramesToDraw(1);
	}
}

void AppContext::Setup() {
	UI::Init();
	Editor::Setup(&project.GetSceneGraph());
	mainWindow = project.CreateEntity("Main Window");
	auto style = Editor::GetStyle();
	mainWindow.Add<Window>(WindowCreateInfo{.name = "NRay", .imGuiStyle = style});
	[[maybe_unused]] auto& windowData = mainWindow.Add<Editor::WindowData>(true);

	mainWindow.Add<WindowImageResource>();
	Window& windowHandle = mainWindow.Get<Window>();
	windowHandle.SetEntityHandle(static_cast<EntityType>(mainWindow.entity));
	
	instance = vb::CreateInstance({
		.validation_layers = true,
		.glfw_extensions = true
	});

	device = instance.CreateDevice({
		.queues = {{{
			.flags = vb::QueueFlagBits::Graphics,
			.present_window = windowHandle.GetGLFWwindow(),
		}}}
	});

	queue = device.GetQueue();

	vb::Descriptor descriptor = device.CreateDescriptor({{
		{vb::DescriptorType::CombinedImageSampler},
		{vb::DescriptorType::StorageBuffer},
	}});

	device.UseDescriptor(descriptor);
	// queue.flags = ;
	// queue.supportedWindowToPresent = windowHandle.GetGLFWwindow();

	materials.Init(device, queue);
	CreateDefaultResources();
	CreateWindowResources(mainWindow);

	cubeVertexBuffer = device.CreateBuffer({
		vertices.size() * sizeof(VertexDebug),
		vb::BufferUsage::Vertex,
		vb::Memory::GPU,
		"Cube Vertex Buffer"
	});
	
	UploadBuffers();
	CreateShaders();
}

} // namespace


// namespace {
// using mat4 = lmath::float4x4;
// enum class MaterialPass :uint8_t {
//     MainColor,
//     Transparent,
//     Other
// };

// struct MaterialInstance {
//     vb::Pipeline pipeline;
//     MaterialPass passType;
// };
// struct RenderObject {
//     uint32_t indexCount;
//     uint32_t firstIndex;
//     vb::Buffer indexBuffer;
    
//     MaterialInstance* material;

//     mat4 transform;
//     VkDeviceAddress vertexBufferAddress;
// };

// struct GLTFMetallic_Roughness {
// 	vb::Pipeline opaquePipeline;
// 	vb::Pipeline transparentPipeline;

// 	// VkDescriptorSetLayout materialLayout;

// 	struct MaterialConstants {
// 		vec4 colorFactors;
// 		vec4 metal_rough_factors;
// 		//padding, we need it anyway for uniform buffers
// 		vec4 extra[14];
// 	};

// 	struct MaterialResources {
// 		vb::Image colorImage;
// 		// VkSampler colorSampler;
// 		vb::Image metalRoughImage;
// 		// VkSampler metalRoughSampler;
// 		vb::Buffer dataBuffer;
// 		uint32_t dataBufferOffset;
// 	};


// 	MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator);
// };

// } // namespace

void AppContext::test() {

	UI::Init();
	Editor::Setup(&project.GetSceneGraph());
	mainWindow = project.CreateEntity("Main Window");
	auto style = Editor::GetStyle();
	mainWindow.Add<Window>(WindowCreateInfo{.name = "NRay", .imGuiStyle = style});
	[[maybe_unused]] auto& windowData = mainWindow.Add<Editor::WindowData>(true);

	mainWindow.Add<WindowImageResource>();
	Window& windowHandle = mainWindow.Get<Window>();
	windowHandle.SetEntityHandle(static_cast<EntityType>(mainWindow.entity));
	
	instance = vb::CreateInstance({
		.validation_layers = true,
		.glfw_extensions = true,
	});
	device = instance.CreateDevice({
		.queues = {{
			{
				.flags = vb::QueueFlagBits::Graphics,
				.present_window = windowHandle.GetGLFWwindow(),
			}
		}}
	});

	queue = device.GetQueue();
	materials.Init(device, queue);
	CreateDefaultResources();
	CreateWindowResources(mainWindow);

	// cubeVertexBuffer = device.CreateBuffer({
	// 	vertices.size() * sizeof(VertexDebug),
	// 	vb::BufferUsage::Vertex,
	// 	vb::Memory::GPU,
	// 	"Cube Vertex Buffer"
	// });
	
	// UploadBuffers();
	// CreateShaders();
	
	device.WaitQueue(queue);
	ctx->project.Destroy();
	UI::Destroy();
}

void FeatureTestApplication::run(char const* gltfPath) {
	Create(gltfPath);
	Setup();
	MainLoop();
	// Draw();
	Finish();
	// ctx = new AppContext();
	// ctx->test();
	// delete ctx;
	// WindowManager::Finish();
}

std::chrono::high_resolution_clock::time_point lastFrameTime = {};

void BatterySaver() {
	float targetFrameTime = 33.333f;
	auto currentTime = std::chrono::high_resolution_clock::now();
	float elapsedTime = 0.0f;
	do {
		elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime).count() / 1000000.0f;
	} while (elapsedTime < targetFrameTime);
	// std::this_thread::sleep_for(std::chrono::duration<float>(targetFrameTime - (currentTime - lastFrameTime).count() / 1000.0f));
	// std::this_thread::sleep_for(std::chrono::duration<float>(targetFrameTime));
	lastFrameTime = std::chrono::high_resolution_clock::now();
}

void FeatureTestApplication::Create(char const* gltfPath) {
	ctx = new AppContext();
	ctx->gltfPath = gltfPath;

	ctx->phongMaterial = ctx->project.CreateEntity("PhongMaterial");
	// phongMaterial.Add<PhongMaterial>();
	Phong::PhongMaterial phongMat = {
		.ambient = vec3(0.1, 0.1, 0.1),
		.diffuse = vec3(0.8, 0.8, 0.8),
		.specular = vec3(0.8, 0.8, 0.8),
		.shininess = 64.0
	};
	ctx->project.GetRegistry().emplace<Phong::PhongMaterial>(ctx->phongMaterial.entity, phongMat);

	Phong::PhongLight light = {
		.position = vec3(10.0, 10.0, 0.0),
		.color = vec3(0.8, 0.8, 0.8)
	};
	ctx->phongLight = ctx->project.CreateEntity("PhongLight");
	ctx->project.GetRegistry().emplace<Phong::PhongLight>(ctx->phongLight.entity, light);
}

void FeatureTestApplication::Setup() {
	ctx->Setup();
}

void DrawOrRemoveWindows() {
	auto registry = &ctx->project.GetRegistry();
	auto view = registry->view<Window>();
	for (auto w: view) {
		auto& windowHandle = registry->get<Window>(w);
		if (windowHandle.GetShouldClose()) {
			ctx->device.WaitIdle();
			windowHandle.Destroy();
			registry->destroy(w);
			if (w == ctx->mainWindow.entity) {
				ctx->mainWindow.entity = Entity::Null;
			}
		} else {
			windowHandle.ApplyChanges();
			// LOG_INFO("Iconified {}", windowHandle.GetIconified());
			if (!windowHandle.GetIconified()) {
				// Recreate swapchain if needed
				if (ctx->swapchains[&windowHandle].GetDirty() || windowHandle.GetFramebufferResized()) {
					LOG_INFO("DIRTY FRAME RESOURCES");
					windowHandle.UpdateFramebufferSize();
					ctx->device.WaitIdle();
					uint2 size = uint2(windowHandle.GetSize());
					ctx->swapchains[&windowHandle].Recreate(size.x, size.y);

					windowHandle.SetFramebufferResized(false);
					windowHandle.AddFramesToDraw(1);
				}
				// Draw if needed
				if (windowHandle.GetDrawNeeded()){
					ctx->DrawWindow({&ctx->project.GetRegistry(), static_cast<entt::entity>(windowHandle.GetEntityHandle())});
				}
			}
		}
	}
}

void FeatureTestApplication::MainLoop() {
	[[maybe_unused]] auto c = ctx;
	lastFrameTime = std::chrono::high_resolution_clock::now();
	while (ctx->mainWindow.entity != Entity::Null) {
		auto& sceneGraph = ctx->project.GetSceneGraph();
		sceneGraph.UpdateTransforms(sceneGraph.root, sceneGraph.root.entity.Get<Component::Transform>());

		auto registry = &ctx->project.GetRegistry();
		auto view = registry->view<Window>();//view->remove(ctx->mainWindow.entity);
		std::any_of( view.begin(), view.end(), [registry](const auto &window) {
			Window& windowHandle = registry->get<Window>(window);
			// std::this_thread::sleep_for(std::chrono::duration<float>(1.0f / 60.0f));
			return windowHandle.GetDrawNeeded();
		}) ? WindowManager::PollEvents() : WindowManager::WaitEvents();
		
		DrawOrRemoveWindows();
	}
	ctx->device.WaitIdle();
}

void FeatureTestApplication::Finish() {
	ctx->project.Destroy();
	UI::Destroy();
	for (auto& window : ctx->project.GetRegistry().view<Window>()) {
		ctx->project.GetRegistry().destroy(window);
	}
	// ctx->errorImage = {};

	// ctx->pipeline = {};
	// ctx->glTFPipeline = {};
	// ctx->texPipeline = {};
	// ctx->opaquePipeline = {};
	// ctx->cubeVertexBuffer = {};

	// ctx->materials = {};


	// ctx->device = {};
	// ctx->instance = {};
	delete ctx;
	// vb::Destroy();
	WindowManager::Finish();
}