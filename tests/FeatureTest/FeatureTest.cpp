#include <cstdint>
#include <cstdio>
#include <set>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <vector>

#include "Log.hpp"
#include "Window.hpp"
#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include "Editor.hpp"
#include "FileManager.hpp"
#include "FeatureTest.hpp"

#include "GLTFLoader.hpp"
#include "Project.hpp"

using namespace Lmath;

using Pixel = vec4;
namespace {
struct AppContext {
	vkw::Pipeline pipeline;

	vkw::Pipeline glLTPipeline;
	// vkw::Pipeline computePipeline;

	uint32_t width, height;

	ImDrawData* imguiDrawData = nullptr;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	vkw::SampleCount sampleCount = vkw::SampleCount::_4;
	// vkw::Format renderFormat = vkw::Format::RGBA16_sfloat;
	vkw::Format renderFormat = vkw::Format::RGBA8_UNORM;
	

	// vkw::Buffer outputImage;
	vkw::Buffer cubeVertexBuffer;
	vkw::Device device;

	vkw::Queue queue;
	// vkw::Image renderImage;

	Window* mainWindow;
	std::set<Window*> windows;
	std::unordered_map<Window*, vkw::Image> colorImages;
	std::unordered_map<Window*, vkw::Image> depthImages;
	std::unordered_map<Window*, vkw::Image> resolveImages;

	Editor editor;

	Project project;

	std::vector<Entity> meshes;

	AppContext () = default;
	AppContext (const AppContext&) = delete;
	AppContext& operator=(const AppContext&) = delete;
	AppContext (AppContext&&) = delete;
	AppContext& operator=(AppContext&&) = delete;
	
	void CreateShaders();
	void CreateWindowResources(Window* window);
	void UploadBuffers();
	void RecordCommands(Window* window);
	void viewport();
	void RenderUI();
	void DrawImgui(Window* window);
	void DrawWindow(Window* window);
	void RecreateFrameResources(Window* window);

};
static AppContext* ctx;

struct Camera {
    vec3 velocity;

	vec3 focus = {0.0f, 0.0f, 0.0f};
	float rotation_factor = 0.0025f;
	float zoom_factor = 0.01f;
	float move_factor = 0.00107f;

    mat4 view = lookAt(vec3(0.0f, 0.0f, -3.0f), focus, vec3(0.0f, 1.0f, 0.0f));
	mat4 proj = perspective(60.0f, 1.0f, 0.1f, 100.0f);


};
static Camera camera;

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
		.depthFormat = depthImages[mainWindow].format,
		.samples = sampleCount,
		.lineTopology = true
	});

	glLTPipeline = device.CreatePipeline({
		.point = vkw::PipelinePoint::Graphics,
		.stages = {
			{.stage = vkw::ShaderStage::Vertex, .path = "tests/FeatureTest/glTF.vert"},
			{.stage = vkw::ShaderStage::Fragment, .path = "tests/FeatureTest/glTF.frag"},
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
		.depthFormat = depthImages[mainWindow].format,
		.samples = sampleCount,
		.lineTopology = false
	});

}

// void AppContext::CreateImages(uint32_t width, uint32_t height) {

// }

void AppContext::CreateWindowResources(Window* window) {
	window->CreateSwapchain(device, queue);
	window->CreateUI(sampleCount);
	windows.emplace(window);
	window->AddFramebufferSizeCallback(FramebufferCallback);
	window->AddMouseButtonCallback(MouseButtonCallback);
	window->AddWindowRefreshCallback(RefreshCallback);
	window->AddKeyCallback(KeyCallback);
	window->AddCursorPosCallback(CursorPosCallback);
	window->AddScrollCallback(ScrollCallback);

	uint2 maxSize = uint2(window->GetSizeLimits().zw());

	depthImages.erase(window);
	depthImages.try_emplace(window, device.CreateImage({
        .width = maxSize.x,
        .height = maxSize.y,
        .format = vkw::Format::D32_sfloat,
        .usage = vkw::ImageUsage::DepthStencilAttachment | vkw::ImageUsage::TransientAttachment,
		.samples = sampleCount,
        .name = "Depth Attachment"
    }));

	resolveImages.erase(window);
	resolveImages.try_emplace(window, device.CreateImage({
		.width = maxSize.x,
		.height = maxSize.y,
		.format = renderFormat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc,
		.name = window->GetName(),
	}));
	
	colorImages.erase(window);
	colorImages.try_emplace(window, device.CreateImage({
		.width = maxSize.x,
		.height = maxSize.y,
		.format = renderFormat,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransientAttachment,
		.samples = sampleCount,
		.name = window->GetName(),
	}));
}


void AppContext::UploadBuffers() {
	auto cmd = device.GetCommandBuffer(queue);
	cmd.BeginCommandBuffer();
	cmd.Copy(cubeVertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	cmd.Barrier({});
	cmd.EndCommandBuffer();
	cmd.QueueSubmit({});

	glTF::Loader loader;
	const char* filepath = "bin/5_cubes.gltf";
	auto res = loader.Load(filepath, project.GetSceneGraph(), device);
	ASSERT(res, "Failed to load gltf file");

	auto& sceneGraph = project.GetSceneGraph();
	meshes.reserve(sceneGraph.registry->view<Component::Mesh>().size());
	for (auto entity : sceneGraph.registry->view<Component::Mesh>()) {
		meshes.push_back(Entity(sceneGraph.registry, entity));
	}
}

void AppContext::RecordCommands(Window* window) {
	auto size = window->GetSize();
	auto glfwWindow = window->GetGLFWwindow();
	vec4 viewport = {0, 0, (float)size.x, (float)size.y};
	FeatureTestConstants constants{};
	constants.view = inverse4x4(camera.view);
	constants.proj = perspective(60.0f, (float)size.x / size.y, 0.01f, 1000.0f);

	// LOG_INFO("Viewport: {}, {}, {}, {}", viewport.x, viewport.y, viewport.z, viewport.w); 

	auto cmd = window->swapChain.GetCommandBuffer();
	cmd.BeginCommandBuffer();
	if (!window->swapChain.AcquireImage()) return;
	vkw::Image& img = window->swapChain.GetCurrentImage();

	// cmd.Barrier(renderImages[window], {vkw::ImageLayout::TransferDst});
	// cmd.ClearColorImage(renderImages[window], {0.7f, 0.0f, 0.4f, 1.0f});
	if (sampleCount == vkw::SampleCount::_1) {
		// cmd.BeginRendering({{{resolveImages[window]}}}, {depthImages[window]}, 1, viewport);
		cmd.BeginRendering({{{resolveImages[window]}}}, {}, 1, viewport);
	} else {
		// cmd.BeginRendering({{{colorImages[window], resolveImages[window]}}}, {depthImages[window]}, 1, viewport);
		cmd.BeginRendering({{{colorImages[window], resolveImages[window]}}}, {}, 1, viewport);
	}
	// cmd.BindPipeline(pipeline);
	
	cmd.BindPipeline(glLTPipeline);

	for (auto& meshNode : meshes) {
		constants.model = meshNode.GetComponent<Component::Transform>().local;
		cmd.PushConstants(&constants, sizeof(constants));
		auto& mesh = meshNode.GetComponent<Component::Mesh>();
		cmd.DrawMesh(mesh->vertexBuffer, mesh->indexBuffer, mesh->indexBuffer.size / sizeof(uint));
	}


	
	// cmd.Draw(vertices.size(), 1, 0, 0);

	cmd.DrawImGui(imguiDrawData);

	cmd.EndRendering();
	
	cmd.Barrier(resolveImages[window], {vkw::ImageLayout::TransferSrc});
	cmd.Barrier(img, {vkw::ImageLayout::TransferDst});
	cmd.Blit(img, resolveImages[window], {0, 0, size.x, size.y}, {0, 0, size.x, size.y});
	cmd.Barrier(img, {vkw::ImageLayout::Present});

}

void AppContext::DrawWindow(Window* window) {
	if (window->GetDrawNeeded() && !window->GetIconified()) {
		window->SetUIContextCurrent();
		editor.BeginFrame();
		editor.Draw();
		imguiDrawData = editor.EndFrame();
		
		RecordCommands(window);
		if (window->swapChain.GetDirty()) {
			LOG_WARN("RecordCommands: Swapchain dirty");
			return;
		}
		window->swapChain.SubmitAndPresent();
		if (window->swapChain.GetDirty()) {
			LOG_WARN("SubmitAndPresent: Swapchain dirty");
			return;
		}
		window->AddFramesToDraw(-1);
		window->SetFrameCount((window->GetFrameCount() + 1) % (1 << 15));
	}
}

void KeyCallback(Window* window, int key, int scancode, int action, int mods) {
	// printf("key: %d, scancode: %d, action: %d, mods: %d\n", key, scancode, action, mods);
	window->AddFramesToDraw(1);
	switch (action){
	case GLFW_PRESS:
		switch (key) {
		case GLFW_KEY_N: {
			static int windowCount = 1;
			std::string name = "w" + std::to_string(windowCount++);
			auto window = new Window(ctx->width, ctx->height, name.c_str());
			ctx->CreateWindowResources(window);
			break;
		}
		case GLFW_KEY_W:
			camera.view = translate4x4({0, 0, -0.02f}) * camera.view;
			break;
		case GLFW_KEY_S:
			camera.view = translate4x4({0, 0, 0.02f}) * camera.view;
			break;
		case GLFW_KEY_A:
			camera.view = translate4x4({0.02f, 0, 0}) * camera.view;
			break;
		case GLFW_KEY_D:
			camera.view = translate4x4({-0.02f, 0, 0}) * camera.view;
			break;
		case GLFW_KEY_G: {
				auto& io = ImGui::GetIO();
				auto& fonts = io.Fonts;
				unsigned char* texData;
				int texWidth, texHeight;
				fonts->GetTexDataAsAlpha8(&texData, &texWidth, &texHeight);
				// printf("Font texture size: %dx%d\n", texWidth, texHeight);
				// printf("Font texture data: %p\n", texData);
				std::vector<uint32_t> texData4(texWidth * texHeight);
				for (int i = 0; i < texWidth * texHeight; ++i) {
					uint32_t pixel = 0;
					pixel |= (texData[i] << 24);
					pixel |= (texData[i] << 16);
					pixel |= (texData[i] << 8);
					pixel |= (texData[i] << 0);
					texData4[i] = pixel;
				}
				FileManager::SaveBMP("font.bmp", texData4.data(), texWidth, texHeight);
				break;
		}
		// case GLFW_KEY_J: {
		// 	ImGuiIO& io = ImGui::GetIO();
		// 	const float fontSize = 17.0f;
		// 	// editor.BeginFrame();
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
		// 	// editor.EndFrame();
		// 	break;
		// }
		default:
			break;
		}
		break;
	case GLFW_RELEASE:
		break;
	case GLFW_REPEAT:
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
	vec3 camera_pos = camera.view.col(3).xyz();
	vec3 camera_right = camera.view.get_col(0).xyz();
	vec3 camera_up = camera.view.get_col(1).xyz();
	vec3 camera_forward = camera.view.get_col(2).xyz();

	window->AddFramesToDraw(1);


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
	

	if (mouse.buttons[GLFW_MOUSE_BUTTON_RIGHT]) {
		switch (mouse.mods)
		{
		case GLFW_MOD_ALT: {
			auto zoom_factor = camera.zoom_factor * length(camera_pos - camera.focus);
			auto movement =  (zoom_factor * mouse.deltaPos.x) * camera_forward;
			camera.view = translate4x4(movement) * camera.view;
			// camera.focus += movement;
			break;
		}
		
		default:
			camera_pos -= camera.focus;
			// camera.view = rotate4x4(camera_up, mouse.deltaPos.x * camera.rotation_factor) * rotate4x4(camera_right, -mouse.deltaPos.y * camera.rotation_factor)  * camera.view; // trackball
			camera.view = rotate4x4Y(mouse.deltaPos.x * camera.rotation_factor) * rotate4x4(camera_right, -mouse.deltaPos.y * camera.rotation_factor)  * camera.view;
			camera_pos += camera.focus;

			break;
		}
		// window->AddFramesToDraw(1);
	}
	if (mouse.buttons[GLFW_MOUSE_BUTTON_MIDDLE]) {
		auto move_factor = camera.move_factor * length(camera_pos - camera.focus);
		auto movement =  move_factor * (camera_up * mouse.deltaPos.y + camera_right * mouse.deltaPos.x); 
		camera.focus += movement;
		camera.view = translate4x4(movement) * camera.view;
		// window->AddFramesToDraw(1);
	}
	if (mouse.buttons[GLFW_MOUSE_BUTTON_LEFT]) {
		// window->AddFramesToDraw(1);
	}
}

void MouseButtonCallback(Window* window, int button, int action, int mods) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		// if (action == GLFW_PRESS) {
			window->AddFramesToDraw(2);
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
	// DEBUG_TRACE("Window {} framebuffer resized to {}x{}", window->GetName(), width, height);
	if (width == 0 || height == 0) {return;}
	// LOG_INFO("RecreateFrameResources {} callback", window->GetName());
	ctx->RecreateFrameResources(window);
}

void RefreshCallback(Window* window) {
	// window->AddFramesToDraw(1);
	// ctx->DrawWindow(window);
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

void FeatureTestApplication::Setup() {
	vkw::Init();

	ctx = new AppContext();

	UI::Init();
	ctx->editor.Setup();
	ctx->width = info->width;
	ctx->height = info->height;

	auto window = new Window(ctx->width, ctx->height, "NRay");
	ctx->queue = {vkw::QueueFlagBits::Graphics | vkw::QueueFlagBits::Compute | vkw::QueueFlagBits::Transfer, window->GetGLFWwindow()};
	ctx->device = vkw::CreateDevice({&ctx->queue});
	ctx->mainWindow = window;
	ctx->CreateWindowResources(window);

	ctx->cubeVertexBuffer = ctx->device.CreateBuffer({
		vertices.size() * sizeof(Vertex),
		vkw::BufferUsage::Vertex,
		vkw::Memory::GPU,
		"Cube Vertex Buffer"
	});
	
	ctx->UploadBuffers();
	ctx->CreateShaders();
}

void RemoveWindow(std::set<Window*>::iterator& it) {
	auto window = *it;
	ctx->depthImages.erase(window);
	ctx->colorImages.erase(window);
	ctx->resolveImages.erase(window);
	it = ctx->windows.erase(it);
	if (window == ctx->mainWindow) {
		ctx->mainWindow = nullptr;
	}
	delete window;
}

static int loopCount = 0;
void FeatureTestApplication::MainLoop() {
	auto c = ctx;
	lastFrameTime = std::chrono::high_resolution_clock::now();
	while (ctx->mainWindow != nullptr) {
		std::any_of( ctx->windows.begin(), ctx->windows.end(), [](const auto &window) {
			return window->GetDrawNeeded();
		}) ? WindowManager::PollEvents() : WindowManager::WaitEvents();

		auto& sceneGraph = ctx->project.GetSceneGraph();
		sceneGraph.UpdateTransforms(sceneGraph.scenes, sceneGraph.scenes.entity.GetComponent<Component::Transform>());

		for (auto it = ctx->windows.begin(); it != ctx->windows.end();) {
			auto window = *it;
			// LOG_INFO("Window: {}, frames to draw: {}", window->GetName(),window->GetFramesToDraw());
			window->ApplyChanges();
			if (!window->GetAlive()) {
				RemoveWindow(it);
				continue;
			}
			ctx->RecreateFrameResources(window);
			ctx->DrawWindow(window);
			++it;
		}
		// BatterySaver();
	}
	ctx->device.WaitIdle();
}

void FeatureTestApplication::Finish() {
	ctx->project.Destroy();
	UI::Destroy();
	for (auto& window : ctx->windows) {
		delete window;
	}

	delete ctx;
	vkw::Destroy();
	WindowManager::Finish();
}