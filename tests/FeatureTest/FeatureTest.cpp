#ifdef USE_MODULES
module FeatureTest;
import glfw;
import imgui;
import Window;
import Lmath;
import VulkanBackend;
import Component;
import stl;
import Log;
import Editor;
import Entity;
import Component;
import Project;
import Objects;
import Phong;
import GLTFLoader;
import entt;
import UI;
import Runtime;
import Types;
#else
#include "Lmath.cppm"
#include "VulkanBackend.cppm"
#include "Window.cppm"
#include "Log.cppm"
#include "Bindless.h"
#include "Editor.cppm"
#include "FeatureTest.cppm"

#include "Phong.cppm"

#include "GLTFLoader.cppm"
#include "Project.cppm"

#include "UI.cppm"

#include <cstdint>
#include <cstdio>
#include <vector>

#include "Types.cppm"

#include "Runtime.cppm"
#endif


using namespace Lmath;

using Pixel = vec4;
namespace {
struct DrawViewportInfo;

struct AppContext : DeleteCopyDeleteMove {
	vkw::Pipeline pipeline;

	vkw::Pipeline glTFPipeline;
	// vkw::Pipeline computePipeline;

	uint32_t width, height;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	vkw::SampleCount sampleCount = vkw::SampleCount::_4;
	// vkw::SampleCount sampleCount = vkw::SampleCount::_1;
	// vkw::Format renderFormat = vkw::Format::RGBA16_sfloat;
	vkw::Format renderFormat = vkw::Format::RGBA8_UNORM;
	

	// vkw::Buffer outputImage;
	vkw::Buffer cubeVertexBuffer;
	vkw::Device device;

	vkw::Queue queue;
	// vkw::Image renderImage;

	Entity mainWindow;


	Entity phongMaterial;
	Entity phongLight;

	// Runtime::Context runtimeContext;

	Project project;
	inline Entity CreateEntity(const std::string_view& name = "") {
		auto entity = Entity(&registry, registry.create());
		entity.Add<Component::Name>(name);
		return entity;
	}

	Registry registry;

	// std::vector<Entity> meshes;

	void CreateShaders();
	void CreateWindowResources(Entity window);
	void UploadBuffers();
	void DrawWindow(Entity window);
	void viewport();
	void RenderUI();
	void DrawImgui(Entity window);
	void DrawViewport(vkw::Command& cmd, Runtime::Camera const& camera);
	void RecreateFrameResources(Window* window);

};
static AppContext* ctx;

struct WindowImageResource : DeleteCopyDeleteMove {
	vkw::Image colorImage;
	vkw::Image depthImage;
	vkw::Image resolveImage;
};

struct DrawViewportInfo {
	vkw::Command&            cmd;
	Window&                  windowHandle;
	WindowImageResource&     resource;
	vkw::Image &              targetImage;
};

constexpr auto fov = 60.0f;
constexpr auto zNear = 0.01f;
constexpr auto zFar = 1000.0f;

// static Runtime::Camera camera(vec3(0.0f, 0.0f, 30.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

struct Vertex {
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
void printMatrix(const float4x4& m, const char* name = nullptr);
// const std::vector<Vertex> vertices = {
// 	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
// 	{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
// 	{{0.5f, 0.5f, 0.0f},  {0.0f, 1.0f, 0.0f}},

// 	{{0.0f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
// 	{{-0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}},
// 	{{0.5f, 0.5f, 1.0f},  {0.0f, 1.0f, 0.0f}},

// };

// Cube
const std::vector<Vertex> vertices = {
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
	pipeline = device.CreatePipeline({
		.point = vkw::PipelinePoint::Graphics,
		.stages = {
			{.stage = vkw::ShaderStage::Vertex, .path = "tests/FeatureTest/FeatureTest.vert"},
			{.stage = vkw::ShaderStage::Fragment, .path = "tests/FeatureTest/FeatureTest.frag"},
		},
		.name = "Feature pipeline",
		// pos2 + color3
		.vertexAttributes = {vkw::Format::RGB32_sfloat, vkw::Format::RGB32_sfloat},
		.colorFormats = {renderFormat},
		.useDepth = true,
		.depthFormat = resource.depthImage.format,
		.samples = sampleCount,
		.lineTopology = false
	});

	glTFPipeline = device.CreatePipeline({
		.point = vkw::PipelinePoint::Graphics,
		.stages = {
			{.stage = vkw::ShaderStage::Vertex, .path = "source/Shaders/Phong.vert"},
			{.stage = vkw::ShaderStage::Fragment, .path = "source/Shaders/Phong.frag"},
		},
		.name = "Feature pipeline",
		// pos2 + color3
		.vertexAttributes = {
			vkw::Format::RGB32_sfloat,  // vec3  position;
			vkw::Format::RGB32_sfloat,  // vec3  normal;
			vkw::Format::RG32_sfloat,   // vec2  uv;
			vkw::Format::RGBA32_sfloat, // vec4  color;
			vkw::Format::RGB32_sfloat   // vec3  tangent;
		},
		.colorFormats = {renderFormat},
		.useDepth = true,
		.depthFormat = resource.depthImage.format,
		.samples = sampleCount,
		.lineTopology = false
	});

}

// void AppContext::CreateImages(uint32_t width, uint32_t height) {

// }

void AppContext::CreateWindowResources(Entity window) {
	Window &windowHandle = window.Get<Window>();
	windowHandle.CreateSwapchain(device, queue);
	windowHandle.CreateUI(sampleCount);
	windowHandle.AddFramebufferSizeCallback(FramebufferCallback);
	windowHandle.AddMouseButtonCallback(MouseButtonCallback);
	windowHandle.AddWindowRefreshCallback(RefreshCallback);
	windowHandle.AddKeyCallback(KeyCallback);
	windowHandle.AddCursorPosCallback(CursorPosCallback);
	windowHandle.AddScrollCallback(ScrollCallback);
	windowHandle.AddWindowIconifyCallback(IconifyCallback);

	uint2 maxSize = uint2(windowHandle.GetSizeLimits().zw());

	auto& resource = window.Get<WindowImageResource>();

	resource.depthImage = device.CreateImage({
        .width = maxSize.x,
        .height = maxSize.y,
        .format = vkw::Format::D32_sfloat,
        .usage = vkw::ImageUsage::DepthStencilAttachment | vkw::ImageUsage::TransientAttachment,
		.samples = sampleCount,
        .name = "Depth Attachment"
    });

	resource.resolveImage = device.CreateImage({
		.width = maxSize.x,
		.height = maxSize.y,
		.format = renderFormat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc,
		.name = windowHandle.GetName(),
	});
	
	resource.colorImage = device.CreateImage({
		.width = maxSize.x,
		.height = maxSize.y,
		.format = renderFormat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransientAttachment,
		.samples = sampleCount,
		.name = windowHandle.GetName(),
	});
}

void AppContext::UploadBuffers() {
	auto cmd = device.GetCommandBuffer(queue);
	cmd.BeginCommandBuffer();
	cmd.Copy(cubeVertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	cmd.Barrier({});
	cmd.EndCommandBuffer();
	cmd.QueueSubmit({});

	glTF::Loader loader;
	const char* filepath = "bin/cubes.gltf";
	auto res = loader.Load(filepath, project.GetSceneGraph(), device);
	ASSERT(res, "Failed to load gltf file");

	auto& sceneGraph = project.GetSceneGraph();
	sceneGraph.DebugPrint();
}

void AppContext::DrawViewport(vkw::Command& cmd, Runtime::Camera const& camera) {

	Phong::PhongConstants constants{};
	constants.viewProj = camera.proj * (camera.view | affineInverse);
	constants.light = phongLight.Get<Phong::PhongLight>();
	constants.material = phongMaterial.Get<Phong::PhongMaterial>();
	constants.cameraPosition = camera.getPosition();

	// LOG_INFO("Viewport: {}, {}, {}, {}", viewport.x, viewport.y, viewport.z, viewport.w); 

	// cmd.Barrier(resource.renderImage, {vkw::ImageLayout::TransferDst});
	// cmd.ClearColorImage(resource.renderImage, {0.7f, 0.0f, 0.4f, 1.0f});

	cmd.BindPipeline(glTFPipeline);
	auto registry = project.GetSceneGraph().registry;
	for (auto& meshNode : registry->view<Component::Mesh>()) {
		constants.model = registry->get<Component::Transform>(meshNode).global;
		cmd.PushConstants(glTFPipeline, &constants, sizeof(constants));
		Component::Mesh& mesh = registry->get<Component::Mesh>(meshNode);
		cmd.DrawMesh(mesh->vertexBuffer, mesh->indexBuffer, mesh->indexBuffer.size / sizeof(uint32_t));
	}

/* 
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


}

void AppContext::DrawWindow(Entity window) {
	auto& windowHandle = window.Get<Window>();
	auto& windowData   = window.Get<Runtime::WindowData>();
	auto& resource     = window.Get<WindowImageResource>();
	auto& cmd          = windowHandle.swapChain.GetCommandBuffer();
	windowHandle.SetUIContextCurrent();

	Editor::BeginFrame();
	Editor::Draw({&ctx->project.GetSceneGraph(), &windowData});
	auto uiDrawData = Editor::EndFrame();

	// ImGuiIO& io = ImGui::GetIO();
	// if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
	// 	ImGui::UpdatePlatformWindows();
	// 	ImGui::RenderPlatformWindowsDefault();
	// }

	cmd.BeginCommandBuffer();
	if (!windowHandle.swapChain.AcquireImage()) {
		LOG_WARN("AcquireImage: Swapchain dirty");
		return;
	}
	auto& targetImage = windowHandle.swapChain.GetCurrentImage();
	
	ivec4 fullWindowRect(0, 0, windowHandle.GetWidth(), windowHandle.GetHeight());
	if (sampleCount == vkw::SampleCount::_1) {
		cmd.BeginRendering({{{{resource.resolveImage}}}, {resource.depthImage}, fullWindowRect});
		// cmd.BeginRendering({{{{resource.resolveImage}}}, {}, fullWindowRect});
	} else {
		cmd.BeginRendering({{{{resource.colorImage, &resource.resolveImage}}}, {resource.depthImage}, fullWindowRect});
		// cmd.BeginRendering({{{{resource.colorImage, resource.resolveImage}}}, {}, fullWindowRect});
	};

	cmd.SetScissor(fullWindowRect);
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
	}

	cmd.DrawImGui(uiDrawData);

	cmd.EndRendering();
	
	cmd.Barrier(resource.resolveImage, {vkw::ImageLayout::TransferSrc});
	cmd.Barrier(targetImage, {vkw::ImageLayout::TransferDst});
	ivec4 blitRegion = fullWindowRect;
	cmd.Blit({
		.dst = targetImage,
		.src = resource.resolveImage,
		.dstRegion = blitRegion,
		.srcRegion = blitRegion,
	});
	cmd.Barrier(targetImage, {vkw::ImageLayout::Present});

	windowHandle.swapChain.SubmitAndPresent();
	if (windowHandle.swapChain.GetDirty()) {
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
		// 	vkw::BeginImGui();
		// 	for (const auto& entry : std::filesystem::directory_iterator("bin")) {
		// 		if (entry.path().extension() == ".ttf" || entry.path().extension() == ".otf") {
		// 			auto font = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), fontSize);
		// 			if (font && entry.path().filename() == "segoeui.ttf") {
		// 				io.FontDefault = font;
		// 			}
		// 			// if (entry.path().filename() == "segoeui.ttf") {
		// 			// 	auto font = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), fontSize);
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

Hit MouseHitTest (Window *window, double xpos, double ypos) {
	// auto& border_thickness = window->GetBorderThickness();
	// l t r b
	Lmath::ivec4 border_thickness = { 50, 30, 50, 50 };
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
	if (size.x == 0 || size.y == 0) {return;}
	window->AddFramesToDraw(1);
	ctx->DrawWindow(Entity(&ctx->registry, static_cast<entt::entity>(window->GetEntityHandle())));
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
	bool swapChainDirty = window->GetSwapchainDirty();
	bool framebufferResized = window->GetFramebufferResized();
	// LOG_INFO("RecreateFrameResources {} {} {} {}", window->GetName(), (void*)window->GetGLFWwindow(), swapChainDirty, framebufferResized);
	if (swapChainDirty || framebufferResized) {
		// LOG_INFO("DIRTY FRAME RESOURCES");
		window->UpdateFramebufferSize();
		window->RecreateSwapchain();
		window->SetFramebufferResized(false);
		window->AddFramesToDraw(1);
	}

	// }
	// ivec2 size = w->GetSize();
	// if (size.x > renderImage.width || size.y > renderImage.height) {
	// 	CreateImages(size.x, size.y);
	// }
	// camera->extent = {viewportSize.x, viewportSize.y};
}

} // namespace


// namespace {
// using mat4 = Lmath::float4x4;
// enum class MaterialPass :uint8_t {
//     MainColor,
//     Transparent,
//     Other
// };

// struct MaterialInstance {
//     vkw::Pipeline pipeline;
//     MaterialPass passType;
// };
// struct RenderObject {
//     uint32_t indexCount;
//     uint32_t firstIndex;
//     vkw::Buffer indexBuffer;
    
//     MaterialInstance* material;

//     mat4 transform;
//     VkDeviceAddress vertexBufferAddress;
// };

// struct GLTFMetallic_Roughness {
// 	vkw::Pipeline opaquePipeline;
// 	vkw::Pipeline transparentPipeline;

// 	// VkDescriptorSetLayout materialLayout;

// 	struct MaterialConstants {
// 		vec4 colorFactors;
// 		vec4 metal_rough_factors;
// 		//padding, we need it anyway for uniform buffers
// 		vec4 extra[14];
// 	};

// 	struct MaterialResources {
// 		vkw::Image colorImage;
// 		// VkSampler colorSampler;
// 		vkw::Image metalRoughImage;
// 		// VkSampler metalRoughSampler;
// 		vkw::Buffer dataBuffer;
// 		uint32_t dataBufferOffset;
// 	};


// 	MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator);
// };

// } // namespace

void FeatureTestApplication::run(FeatureTestInfo* pFeatureTestInfo) {
	info = pFeatureTestInfo;
	Create();
	Setup();
	MainLoop();
	// Draw();
	Finish();
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

void FeatureTestApplication::Create() {
	ctx = new AppContext();

	ctx->phongMaterial = ctx->project.CreateEntity("PhongMaterial");
	// phongMaterial.Add<PhongMaterial>();
	Phong::PhongMaterial phongMat = {
		.ambient = vec3(0.1, 0.1, 0.1),
		.diffuse = vec3(0.8, 0.8, 0.8),
		.specular = vec3(0.8, 0.8, 0.8),
		.shininess = 64.0
	};
	ctx->project.registry.emplace<Phong::PhongMaterial>(ctx->phongMaterial.entity, phongMat);

	Phong::PhongLight light = {
		.position = vec3(10.0, 10.0, 0.0),
		.color = vec3(0.8, 0.8, 0.8)
	};
	ctx->phongLight = ctx->project.CreateEntity("PhongLight");
	ctx->project.registry.emplace<Phong::PhongLight>(ctx->phongLight.entity, light);
}

void FeatureTestApplication::Setup() {
	vkw::Init();

	UI::Init();
	ctx->width = info->width;
	ctx->height = info->height;
	Editor::Setup(&ctx->project.GetSceneGraph());

	// ctx->runtimeContext.camera = &camera;
	// ctx->runtimeContext.sceneGraph = &ctx->project.GetSceneGraph();

	// camera.setProj(fov, ctx->width, ctx->height, zNear, zFar);

	// auto window = new Window(ctx->width, ctx->height, "NRay");
	auto mainWindow = ctx->CreateEntity("Main Window");
	auto style = Editor::GetStyle();
	mainWindow.Add<Window>(ctx->width, ctx->height, "NRay", style);
	auto& windowData = mainWindow.Add<Runtime::WindowData>(true);
	// Editor::SetupWindow(windowData);
	mainWindow.Add<WindowImageResource>();
	Window& windowHandle = mainWindow.Get<Window>();
	windowHandle.SetEntityHandle(static_cast<EntityType>(mainWindow.entity));
	
	ctx->queue = {vkw::QueueFlagBits::Graphics | vkw::QueueFlagBits::Compute | vkw::QueueFlagBits::Transfer, windowHandle.GetGLFWwindow()};
	ctx->device = vkw::CreateDevice({&ctx->queue});
	ctx->mainWindow = mainWindow;
	ctx->CreateWindowResources(mainWindow);

	ctx->cubeVertexBuffer = ctx->device.CreateBuffer({
		vertices.size() * sizeof(Vertex),
		vkw::BufferUsage::Vertex,
		vkw::Memory::GPU,
		"Cube Vertex Buffer"
	});
	
	ctx->UploadBuffers();
	ctx->CreateShaders();
}

void DrawOrRemoveWindows() {
	auto registry = &ctx->registry;
	auto view = registry->view<Window>();
	for (auto w: view) {
		auto& windowHandle = registry->get<Window>(w);
		auto& resource = registry->get<WindowImageResource>(w);
		windowHandle.ApplyChanges();
		if (windowHandle.GetAlive()) {
			// LOG_INFO("Iconified {}", windowHandle.GetIconified());
			if (!windowHandle.GetIconified()) {
				// Recreate swapchain if needed
				if (windowHandle.GetSwapchainDirty() || windowHandle.GetFramebufferResized()) {
					LOG_INFO("DIRTY FRAME RESOURCES");
					windowHandle.UpdateFramebufferSize();
					ctx->device.WaitIdle();
					windowHandle.RecreateSwapchain();
					windowHandle.SetFramebufferResized(false);
					windowHandle.AddFramesToDraw(1);
				}
				// Draw if needed
				if (windowHandle.GetDrawNeeded()){
					ctx->DrawWindow({&ctx->registry, static_cast<entt::entity>(windowHandle.GetEntityHandle())});
				}
			}
		} else {
			registry->destroy(w);
			if (w == ctx->mainWindow.entity) {
				ctx->mainWindow.entity = Entity::Null;
			}
			// window->Destroy();
		}
	}
}

static int loopCount = 0;
void FeatureTestApplication::MainLoop() {
	auto c = ctx;
	lastFrameTime = std::chrono::high_resolution_clock::now();
	while (ctx->mainWindow.entity != Entity::Null) {
		// std::any_of( ctx->windows.begin(), ctx->windows.end(), [](const auto &window) {
		// ctx->registry.any_of( ctx->windows.begin(), ctx->windows.end(), [](const auto &window) {
		auto& sceneGraph = ctx->project.GetSceneGraph();
		sceneGraph.UpdateTransforms(sceneGraph.root, sceneGraph.root.entity.Get<Component::Transform>());

		auto registry = &ctx->registry;
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
	for (auto& window : ctx->registry.view<Window>()) {
		ctx->registry.destroy(window);
	}
	

	delete ctx;
	vkw::Destroy();
	WindowManager::Finish();
}

void printMatrix(const float4x4& m, const char* name) {
	if (name) {
		printf("  %s: \n", name);
	}
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			printf("    % .6f ", m[i][j]);
		}
		printf("\n");
	}
}