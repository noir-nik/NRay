#include "Log.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"

#include "FeatureTest.hpp"
#include <set>
#include "Window.hpp"

#include <unistd.h>

using namespace Lmath;

using Pixel = vec4;
namespace {
struct Context {
	vkw::Pipeline pipeline;
	// vkw::Pipeline computePipeline;

	uint32_t width, height;

	// float4x4 viewMat;
	// float4x4 worldViewInv;
	// float4x4 worldViewProjInv;

	// vkw::Buffer outputImage;
	vkw::Buffer vertexBuffer;

	vkw::Image depth;

	vkw::Device device;

	vkw::Queue queue;
	vkw::Queue computeQueue;
	vkw::Queue transferQueue;
	vkw::Queue transferQueue2;
	vkw::Queue transferQueue3;
	
	// vkw::Image renderImage;

	Window* mainWindow;
	std::set<Window*> windows;
	std::unordered_map<Window*, vkw::Image> renderImages;

	void CreateImages(uint32_t width, uint32_t height);
	void CreateShaders();
};
static Context ctx;

struct Camera {
    vec3 velocity;

	vec3 focus = {0.0f, 0.0f, 0.0f};
	float rotation_factor = 0.0025f;
	float zoom_factor = 0.01f;
	float move_factor = 0.0011f;

    mat4 view = lookAt(vec3(0.0f, 0.0f, -3.0f), focus, vec3(0.0f, 1.0f, 0.0f));
	mat4 proj = perspective(60.0f, 1.0f, 0.1f, 100.0f);


};
static Camera camera;

struct Vertex {
	vec3 pos;
	vec3 color;
};

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

void Context::CreateShaders() {
	pipeline = ctx.device.CreatePipeline({
		.point = vkw::PipelinePoint::Graphics,
		.stages = {
			{.stage = vkw::ShaderStage::Vertex, .path = "tests/FeatureTest/FeatureTest.vert"},
			{.stage = vkw::ShaderStage::Fragment, .path = "tests/FeatureTest/FeatureTest.frag"},
		},
		.name = "Feature pipeline",
		// pos2 + color3
		.vertexAttributes = {vkw::Format::RGB32_sfloat, vkw::Format::RGB32_sfloat},
		// .colorFormats = {ctx.albedo.format, ctx.normal.format, ctx.material.format, ctx.emission.format},
		.colorFormats = {vkw::Format::RGBA16_sfloat},
		// .colorFormats = {vkw::Format::RGBA8_unorm},
		.useDepth = true,
		.depthFormat = ctx.depth.format
	});

}

void Context::CreateImages(uint32_t width, uint32_t height) {
	ctx.depth = ctx.device.CreateImage({
        .width = 3000,
        .height = 3000,
        .format = vkw::Format::D32_sfloat,
        .usage = vkw::ImageUsage::DepthStencilAttachment/*  | vkw::ImageUsage::TransientAttachment */,
        .name = "Depth Attachment"
    });
}

void CreateRenderImage(Window* window) {
	// uint32_t width = window->GetMonitorWidth();
	// uint32_t height = window->GetMonitorHeight();
	uint32_t width = 3000;
	uint32_t height = 3000;
	ctx.renderImages.erase(window);
	ctx.renderImages.try_emplace(window, ctx.device.CreateImage({
		.width = width,
		.height = height,
		.format = vkw::Format::RGBA16_sfloat,
		// .format = vkw::Format::RGBA8_unorm,
		.usage = vkw::ImageUsage::ColorAttachment | vkw::ImageUsage::TransferSrc | vkw::ImageUsage::TransferDst,
		.name = window->GetName(),
	}));
}

void RecreateFrameResources(Window* w);
void UploadBuffers();

void FramebufferCallback(Window* window, int width, int height);
void RefreshCallback(Window* window);
void MouseButtonCallback(Window* window, int button, int action, int mods);
void KeyCallback(Window* window, int key, int scancode, int action, int mods);
void CursorPosCallback(Window *window, double xpos, double ypos);


void UploadBuffers() {
	auto cmd = ctx.device.GetCommandBuffer(ctx.queue);
	cmd.BeginCommandBuffer();
	cmd.Copy(ctx.device, ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	cmd.Barrier({});
	cmd.EndCommandBuffer();
	cmd.QueueSubmit({});
}

void RecordCommands(Window* window) {
	auto size = window->GetSize();
	auto glfwWindow = window->GetGLFWwindow();
	vec4 viewport = {0, 0, (float)size.x, (float)size.y};
	FeatureTestConstants constants{};
	constants.model = float4x4();
	constants.view = inverse4x4(camera.view);
	constants.proj = perspective(60.0f, (float)size.x / size.y, 0.01f, 100.0f);

	// LOG_INFO("Viewport: {}, {}, {}, {}", viewport.x, viewport.y, viewport.z, viewport.w); 

	auto cmd = window->swapChain.GetCommandBuffer();
	cmd.BeginCommandBuffer();
	if (!window->swapChain.AcquireImage()) return;
	vkw::Image& img = window->swapChain.GetCurrentImage();
	
	// cmd.Copy(ctx.vertexBuffer, (void*)vertices.data(), vertices.size() * sizeof(Vertex));
	// cmd.Barrier(ctx.renderImages[window], {vkw::ImageLayout::TransferDst});
	// cmd.ClearColorImage(ctx.renderImages[window], {0.7f, 0.0f, 0.4f, 1.0f});

	cmd.BeginRendering({ctx.renderImages[window]}, {ctx.depth}, 1, viewport);
	cmd.BindPipeline(ctx.pipeline);
	cmd.PushConstants(&constants, sizeof(constants));
	cmd.BindVertexBuffer(ctx.vertexBuffer);
	cmd.Draw(vertices.size(), 1, 0, 0);
	cmd.EndRendering();
	
	cmd.Barrier(ctx.renderImages[window], {vkw::ImageLayout::TransferSrc});
	cmd.Barrier(img, {vkw::ImageLayout::TransferDst});
	cmd.Blit(img, ctx.renderImages[window], {0, 0, size.x, size.y}, {0, 0, size.x, size.y});
	cmd.Barrier(img, {vkw::ImageLayout::Present});

}

void DrawWindow(Window* window) {
	if (window->GetDrawNeeded() && !window->GetIconified()) {
		RecordCommands(window);
		if (window->swapChain.GetDirty()) {
			LOG_WARN("RecordCommands: Swapchain dirty");
		}
		window->swapChain.SubmitAndPresent();
		if (window->swapChain.GetDirty()) {
			LOG_WARN("SubmitAndPresent: Swapchain dirty");
		}
		window->SetDrawNeeded(false);
		window->SetFrameCount((window->GetFrameCount() + 1) % (1 << 15));
	}
}

void KeyCallback(Window* window, int key, int scancode, int action, int mods) {
	// printf("key: %d, scancode: %d, action: %d, mods: %d\n", key, scancode, action, mods);
	switch (action){
	case GLFW_PRESS:
		window->SetDrawNeeded(true);
		switch (key) {
		case GLFW_KEY_N: {
			static int windowCount = 1;
			std::string name = "w" + std::to_string(windowCount++);
			auto window = new Window(ctx.width, ctx.height, name.c_str());
			window->CreateSwapchain(ctx.device, ctx.queue);
			CreateRenderImage(window);
			ctx.windows.emplace(window);
			window->AddFramebufferSizeCallback(FramebufferCallback);
			window->AddMouseButtonCallback(MouseButtonCallback);
			window->AddWindowRefreshCallback(RefreshCallback);
			window->AddKeyCallback(KeyCallback);
			window->AddCursorPosCallback(CursorPosCallback);
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


void CursorPosCallback (Window *window, double xpos, double ypos) {
	vec3 camera_pos = camera.view.col(3).xyz();
	vec3 camera_right = camera.view.get_col(0).xyz();
	vec3 camera_up = camera.view.get_col(1).xyz();
	vec3 camera_forward = camera.view.get_col(2).xyz();

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
		window->SetDrawNeeded(true);
	}
	if (mouse.buttons[GLFW_MOUSE_BUTTON_MIDDLE]) {
		auto move_factor = camera.move_factor * length(camera_pos - camera.focus);
		auto movement =  move_factor * (camera_up * mouse.deltaPos.y + camera_right * mouse.deltaPos.x); 
		camera.focus += movement;
		camera.view = translate4x4(movement) * camera.view;
		window->SetDrawNeeded(true);
	}
}

void MouseButtonCallback(Window* window, int button, int action, int mods) {

}

void FramebufferCallback(Window* window, int width, int height) {
	// DEBUG_TRACE("Window {} framebuffer resized to {}x{}", window->GetName(), width, height);
	if (width == 0 || height == 0) {return;}
	// LOG_INFO("RecreateFrameResources {} callback", window->GetName());
	RecreateFrameResources(window);
}

void RefreshCallback(Window* window) {
	window->SetDrawNeeded(true);
	DrawWindow(window);
}

void RecreateFrameResources(Window* window) {
	if (!window->GetAlive()) { /* LOG_WARN("RecreateFrameResources: Window is dead"); */ return;} // important
	if (window->GetIconified()) {/* LOG_TRACE("RecreateFrameResources: size = 0"); */ return;};

	ctx.device.WaitIdle();
	bool swapChainDirty = window->GetSwapchainDirty();
	bool framebufferResized = window->GetFramebufferResized();
	// LOG_INFO("RecreateFrameResources {} {} {} {}", window->GetName(), (void*)window->GetGLFWwindow(), swapChainDirty, framebufferResized);
	if (swapChainDirty || framebufferResized) {
		// LOG_INFO("DIRTY FRAME RESOURCES");
		window->UpdateFramebufferSize();
		window->RecreateSwapchain();
		window->SetFramebufferResized(false);
		window->SetDrawNeeded(true);
	}

	// }
	// ivec2 size = w->GetSize();
	// if (size.x > ctx.renderImage.width || size.y > ctx.renderImage.height) {
	// 	ctx.CreateImages(size.x, size.y);
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
	Create();
	MainLoop();
	// Draw();
	Finish();
}

void FeatureTestApplication::Setup() {
	ctx.width = info->width;
	ctx.height = info->height;
}

void FeatureTestApplication::Create() { auto& c = ctx;
	vkw::Init();
	auto window = new Window(ctx.width, ctx.height, "wm");
	ctx.queue = {vkw::QueueFlagBits::Graphics | vkw::QueueFlagBits::Compute | vkw::QueueFlagBits::Transfer, window->GetGLFWwindow()};
	ctx.computeQueue = {vkw::QueueFlagBits::Compute, nullptr, true};
	ctx.transferQueue = {vkw::QueueFlagBits::Transfer, nullptr, true};
	ctx.transferQueue2 = {vkw::QueueFlagBits::OpticalFlow, nullptr, true};
	ctx.transferQueue3 = {vkw::QueueFlagBits::OpticalFlow, nullptr, true};
	ctx.device = vkw::CreateDevice({&ctx.queue, &ctx.computeQueue, &ctx.transferQueue2, &ctx.transferQueue3, &ctx.transferQueue});
	window->CreateSwapchain(ctx.device, ctx.queue);
	ctx.windows.emplace(window);
	ctx.mainWindow = window;
	window->AddFramebufferSizeCallback(FramebufferCallback);
	window->AddWindowRefreshCallback(RefreshCallback);
	window->AddMouseButtonCallback(MouseButtonCallback);
	window->AddKeyCallback(KeyCallback);
	window->AddCursorPosCallback(CursorPosCallback);
	ctx.CreateImages(window->GetMonitorWidth(), window->GetMonitorHeight());
	CreateRenderImage(window);
	ctx.vertexBuffer = ctx.device.CreateBuffer(vertices.size() * sizeof(Vertex), vkw::BufferUsage::Vertex, vkw::Memory::GPU, "Vertex Buffer");
	UploadBuffers();
	ctx.CreateShaders();
}
static int loopCount = 0;
void FeatureTestApplication::MainLoop() {
	Context* c = &ctx;
	while (ctx.mainWindow != nullptr) {
		WindowManager::WaitEvents();
		for (auto it = ctx.windows.begin(); it != ctx.windows.end();) {
			auto window = *it;
			window->ApplyChanges();
			if (!window->GetAlive()) {
				ctx.renderImages.erase(window);
				it = ctx.windows.erase(it);
				if (window == ctx.mainWindow) {
					ctx.mainWindow = nullptr;
				}
				delete window;
				continue;
			}
			// LOG_INFO("RecreateFrameResources {}", window->GetName());
			RecreateFrameResources(window);
			++it;
		}
		for (auto& window : ctx.windows) {
            DrawWindow(window);
        }
	}
	ctx.device.WaitIdle();
}

void FeatureTestApplication::Finish() {
	for (auto& window : ctx.windows) {
		delete window;
	}
	ctx = {};
	vkw::Destroy();
	WindowManager::Finish();
}