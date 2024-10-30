#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"

#define SHADER_ALWAYS_COMPILE 0

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#define IMGUI_VULKAN_DEBUG_REPORT
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

static const char *VK_ERROR_STRING(VkResult result);
namespace vkw {

struct InternalQueue;
struct SwapChain;
struct Context
{	
	// void EndCommandBuffer(VkSubmitInfo submitInfo);
	// Command::void EndCommandBuffer(VkSubmitInfo submitInfo);

	void LoadShaders(Pipeline& pipeline);
	std::vector<char> CompileShader(const std::filesystem::path& path, const char* entryPoint);
	void CreatePipelineImpl(const PipelineDesc& desc, Pipeline& pipeline);

	VkInstance instance = VK_NULL_HANDLE;
	VkSurfaceKHR _dummySurface = VK_NULL_HANDLE; // for querying present support
	VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
	std::string applicationName = "Vulkan Slang Compute";
	std::string engineName = "Vulkan Compute";
	VmaAllocator vmaAllocator;

	// VkPhysicalDeviceProperties physicalDeviceProperties;
	// VkPhysicalDeviceFeatures physicalDeviceFeatures;
	// uint32_t presentQueueFamilyIndex;
	// VkDevice device = VK_NULL_HANDLE;
	// VkCommandPool commandPool;
	// VkCommandBuffer *commandBuffers;
	// VkFence *fences;

	uint32_t apiVersion;
	bool presentRequested = false;
	bool enableValidationLayers = true;
	// bool enableValidationLayers = false;
	bool enableDebugReport = false;
	std::vector<bool> activeLayers; // Available layers
	std::vector<const char*> activeLayersNames;
	std::vector<VkLayerProperties> layers;
	std::vector<bool> activeExtensions;                    // Instance Extensions
	std::vector<const char*> activeExtensionsNames;	       // Instance Extensions
	std::vector<VkExtensionProperties> instanceExtensions; // Instance Extensions


	std::vector<const char*> requiredExtensions = { // Physical Device Extensions
		// VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
		// VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		// VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		// VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		// VK_KHR_RAY_QUERY_EXTENSION_NAME,
		// VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
	};

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkSampleCountFlagBits maxSamples = VK_SAMPLE_COUNT_1_BIT;
	VkSampleCountFlags sampleCounts;

	bool descriptorIndexingAvailable = false;
	
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures;
	VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures;
	VkPhysicalDeviceFeatures2 physicalFeatures2{};
	VkPhysicalDeviceProperties physicalProperties{};
	// VkPhysicalDeviceImageFormatInfo2 imageFormatInfo2{};
	// VkImageFormatProperties2 imageFormatProperties2{};
	
	std::vector<VkExtensionProperties> availableExtensions; // Physical Device Extensions
	std::vector<VkQueueFamilyProperties> availableFamilies;

	const uint32_t timeStampPerPool = 64;
		
	bool requestSeparate[Queue::Count] = {false};
	std::unordered_map<uint32_t, InternalQueue> uniqueQueues;
	// Owns all command resources outside swapchains
	// Do not resize after creation!
	std::vector<Command> queuesCommands;
	InternalQueue* queues[Queue::Count]; // Pointers to uniqueQueues
	// Queue currentQueue = Queue::Count;

	std::shared_ptr<PipelineResource> currentPipeline;
	const uint32_t stagingBufferSize = 2 * 1024 * 1024;

	VkPhysicalDeviceMemoryProperties memoryProperties;


	// bindless resources
	VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet bindlessDescriptorSet = VK_NULL_HANDLE;
	VkDescriptorPool bindlessDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout bindlessDescriptorLayout = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;

	GLFWwindow* currentWindow = nullptr;
	// std::unordered_map<GLFWwindow*, SwapChain> swapChains;

	// VkFormat depthFormat;
	

	std::vector<int32_t> availableBufferRID;
	std::vector<int32_t> availableImageRID;
	std::vector<int32_t> availableTLASRID;
	VkSampler genericSampler;

	

	// const uint32_t initialScratchBufferSize = 64*1024*1024;
	// Buffer asScratchBuffer;
	// VkDeviceAddress asScratchAddress;
	// Buffer dummyVertexBuffer;


	// std::map<std::string, float> timeStampTable;

	void CreateInstance(GLFWwindow* window);
	void DestroyInstance();

	void CreatePhysicalDevice();

	void CreateDevice();
	void DestroyDevice();

	void CreateImGui(GLFWwindow* window);
	void DestroyImGui();

	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSet();
	void createDescriptorResources(); // ALL:pool+layout+set
	void destroyDescriptorResources();

	void createBindlessResources();
	void destroyBindlessResources();

	void CreateCommandBuffers(std::vector<Command>& commands);
	void DestroyCommandBuffers(std::vector<Command>& commands);

	void AcquireStagingBuffer();
	void ReleaseStagingBuffer();

	VkSampleCountFlagBits numSamples  = VK_SAMPLE_COUNT_1_BIT;
	VkSampler CreateSampler(VkDevice device, f32 maxLod);

	PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
	// PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
	// PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
	// PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
	// PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
	// PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
	// PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;

	VkDescriptorPool      descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet       descriptorSet = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

};
static Context _ctx;

struct Device {

};

struct SwapChainResource {
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;	
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> swapChainImageResources;
	std::vector<VkImageView> swapChainViews;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkPresentModeKHR> availablePresentModes;
	std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;
	// // preferred, warn if not available
	VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	// VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
	VkColorSpaceKHR colorSpace  = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	VkExtent2D extent;

	// inline VkSemaphore GetImageAvailableSemaphore() { return imageAvailableSemaphores[currentFrame]; }
	// inline VkSemaphore GetRenderFinishedSemaphore() { return renderFinishedSemaphores[currentFrame]; }
	bool SupportFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
	VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
	VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
};
class SwapChain {
	std::shared_ptr<SwapChainResource> resource;

	std::vector<Image> swapChainImages;
	std::vector<Command> commands; // Owns all command resources

	GLFWwindow* window = nullptr;	
	uint32_t framesInFlight = 2;
	uint32_t additionalImages = 0;
	uint32_t currentFrame = 0;
	bool dirty = true;
	uint32_t currentImageIndex = 0;

	// SwapChain() = default;
	SwapChain(GLFWwindow* window, uint32_t width, uint32_t height);
	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;
	~SwapChain() { // TODO: check if this is possible
		// printf("~SwapChain\n");
	}

	inline Image&      GetCurrentSwapChainImage()   { return swapChainImages[currentImageIndex];     }
	Command&           GetCommandBuffer()           { return commands[currentFrame];        }

	void CreateImGui(GLFWwindow* window);
	void DestroyImGui();

	void Recreate(uint32_t width, uint32_t height);

	void CreateSurfaceFormats();
public:
	void Create(GLFWwindow* window, uint32_t width, uint32_t height, bool is_recreation = false);
	void Destroy(bool is_recreation = false);
};

struct Resource {
	std::string name;
	int32_t rid = -1;
	virtual ~Resource() {};
};

struct BufferResource : Resource {
	VkBuffer buffer;
	VmaAllocation allocation;

	virtual ~BufferResource() {
		vmaDestroyBuffer(_ctx.vmaAllocator, buffer, allocation);
		// printf("destroy buffer %s\n", name.c_str());
		if (rid >= 0) {
			_ctx.availableBufferRID.push_back(rid);
			rid = -1;
		}
	}
};

struct ImageResource : Resource {
	VkImage image;
	VkImageView view;
	VmaAllocation allocation;
	bool fromSwapchain = false;
	std::vector<VkImageView> layersView;
	// std::vector<ImTextureID> imguiRIDs;

	virtual ~ImageResource() {
		if (!fromSwapchain) {
			// printf("destroy image %s\n", name.c_str());
			for (VkImageView layerView : layersView) {
				vkDestroyImageView(_ctx.device, layerView, _ctx.allocator);
			}
			layersView.clear();
			vkDestroyImageView(_ctx.device, view, _ctx.allocator);
			vmaDestroyImage(_ctx.vmaAllocator, image, allocation);
			if (rid >= 0) {
				_ctx.availableImageRID.push_back(rid);
				// for (ImTextureID imguiRID : imguiRIDs) {
				//     ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)imguiRID);
				// }
				rid = -1;
				// imguiRIDs.clear();
			}
		} else {
			// printf("FROM SWAPCHAIN %s\n", name.c_str());
		}
	}
};

struct PipelineResource : Resource {
	VkPipeline pipeline;
	VkPipelineLayout layout;

	virtual ~PipelineResource() {
		vkDestroyPipeline(_ctx.device, pipeline, _ctx.allocator);
		vkDestroyPipelineLayout(_ctx.device, layout, _ctx.allocator);
	}
};

struct CommandResource {
	u8* stagingCpu = nullptr;
	uint32_t stagingOffset = 0;
	Buffer staging;

	InternalQueue* queue = nullptr;
	PipelineResource* currentPipeline = nullptr;

	VkCommandPool pool = VK_NULL_HANDLE;
	VkCommandBuffer buffer = VK_NULL_HANDLE;
	VkFence fence = VK_NULL_HANDLE;
	VkQueryPool queryPool;
	std::vector<std::string> timeStampNames;
	std::vector<uint64_t> timeStamps;
};

struct InternalQueue {
	int family = -1;
	uint32_t indexInFamily = 0;
	VkQueue queue = VK_NULL_HANDLE;
	Command* commands = nullptr;
};


uint32_t Buffer::RID() {
	DEBUG_ASSERT(resource->rid != -1, "Invalid buffer rid");
	return uint32_t(resource->rid);
}

// uint32_t TLAS::RID() {
//     DEBUG_ASSERT(resource->rid != -1, "Invalid tlas rid");
//     return uint32_t(resource->rid);
// }

uint32_t Image::RID() {
	DEBUG_ASSERT(resource->rid != -1, "Invalid image rid");
	return uint32_t(resource->rid);
}

static void InitImpl(GLFWwindow* window, uint32_t width, uint32_t height){

	_ctx.CreateInstance(window);
	_ctx.CreatePhysicalDevice();
	_ctx.CreateDevice();

	_ctx.createBindlessResources();
	// _ctx.createDescriptorResources();

	// if (_ctx.presentRequested) {
	// 	_ctx.swapChains.try_emplace(window);
	// 	_ctx.swapChains[window].Create(window, width, height, false);
	// 	// _ctx.CreateImGui(window);
	// }
	
	// Command pools and buffers just for queues
	_ctx.queuesCommands.resize(_ctx.uniqueQueues.size());
	uint32_t i = 0;
	for (auto& [key, q]: _ctx.uniqueQueues) {
		q.commands = _ctx.queuesCommands.data() + i;
		q.commands->resource->queue = &q;
		i++;
		auto& cmd = *(q.commands);
		// resource->staging = CreateBuffer(_ctx.stagingBufferSize, BufferUsage::TransferSrc, Memory::CPU, "StagingBuffer[" + std::to_string(resource->queueFamilyIndex) + "]");
		// resource->stagingCpu = (u8*)resource->staging.resource->allocation->GetMappedData();
	}
	_ctx.CreateCommandBuffers(_ctx.queuesCommands);
}

void Init() {
	InitImpl(nullptr, 0, 0);
}

void Init(GLFWwindow* window, uint32_t width, uint32_t height) {
	_ctx.presentRequested = true;
	InitImpl(window, width, height);
}

void ImGuiCheckVulkanResult(VkResult res) {
    if (res == 0) {
        return;
    }
    std::cerr << "vulkan error during some imgui operation: " << res << '\n';
    if (res < 0) {
        throw std::runtime_error("");
    }
}

void SwapChain::CreateImGui(GLFWwindow* window) {
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = _ctx.instance;
    initInfo.PhysicalDevice = _ctx.physicalDevice;
    initInfo.Device = _ctx.device;
    initInfo.QueueFamily = _ctx.queues[vkw::Queue::Graphics]->family;
    initInfo.Queue = _ctx.queues[vkw::Queue::Graphics]->queue;
    initInfo.DescriptorPool = _ctx.imguiDescriptorPool;
    initInfo.MinImageCount = resource->surfaceCapabilities.minImageCount;
    initInfo.ImageCount = (uint32_t)swapChainImages.size();
    initInfo.MSAASamples = _ctx.numSamples;
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.UseDynamicRendering = true;
    initInfo.Allocator = _ctx.allocator;
    initInfo.CheckVkResultFn = ImGuiCheckVulkanResult;
    ImGui_ImplVulkan_Init(&initInfo);
    ImGui_ImplVulkan_CreateFontsTexture();
    ImGui_ImplGlfw_InitForVulkan(window, true);
}

void SwapChain::DestroyImGui() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void SwapChain::Recreate(GLFWwindow* window, uint32_t width, uint32_t height) {
	DEBUG_ASSERT(!(width == 0 || height == 0), "Window size is 0, swapchain NOT recreated");
	if (_ctx.swapChains.find(window) == _ctx.swapChains.end()) {
		// DEBUG_TRACE("RecreateSwapChain: {} not found", (void*)window);
		_ctx.swapChains.try_emplace(window);
		_ctx.swapChains[window].Create(_ctx.device, window, width, height);
	} else {
		auto& swapChain = _ctx.swapChains.at(window);
		// DEBUG_TRACE("RecreateSwapChain: {}", (void*)window);
		swapChain.Destroy(true);
		swapChain.Create(_ctx.device, window, width, height, true);
	}
	// LOG_INFO("Swapchain recreated!");
}

void DestroySwapChain(GLFWwindow* window) {
	if (_ctx.swapChains.find(window) == _ctx.swapChains.end()){
		LOG_WARN("DestroySwapChain: Swapchain not found!");
		return;
	}
	auto& swapChain = _ctx.swapChains[window];
	swapChain.Destroy();
	_ctx.swapChains.erase(window);
	// DEBUG_TRACE("DestroySwapChain {} size =  {}", (void*)window, _ctx.swapChains.size());
}

void Destroy() {
	// ImGui_ImplVulkan_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// for (auto& [key, swapChain] : _ctx.swapChains) {
	// 	swapChain.Destroy();
	// }

	_ctx.destroyBindlessResources();
	// _ctx.destroyDescriptorResources();

	_ctx.DestroyCommandBuffers(_ctx.queuesCommands);
	_ctx.DestroyDevice();
	_ctx.DestroyInstance();
}

void* MapBuffer(Buffer& buffer) {
	ASSERT(buffer.memory & Memory::CPU, "Buffer not cpu accessible!");
	void* data;
	vmaMapMemory(_ctx.vmaAllocator, buffer.resource->allocation, &data);
	return buffer.resource->allocation->GetMappedData();
}

void UnmapBuffer(Buffer& buffer) {
	ASSERT(buffer.memory & Memory::CPU, "Buffer not cpu accessible!");
	vmaUnmapMemory(_ctx.vmaAllocator, buffer.resource->allocation);
}


Buffer CreateBuffer(uint32_t size, BufferUsageFlags usage, MemoryFlags memory, const std::string& name) {
	if (usage & BufferUsage::Vertex) {
		usage |= BufferUsage::TransferDst;
	}

	if (usage & BufferUsage::Index) {
		usage |= BufferUsage::TransferDst;
	}

	if (usage & BufferUsage::Storage) {
		usage |= BufferUsage::Address;
		size += size % _ctx.physicalProperties.limits.minStorageBufferOffsetAlignment;
	}

	if (usage & BufferUsage::AccelerationStructureInput) {
		usage |= BufferUsage::Address;
		usage |= BufferUsage::TransferDst;
	}

	if (usage & BufferUsage::AccelerationStructure) {
		usage |= BufferUsage::Address;
	}

	std::shared_ptr<BufferResource> res = std::make_shared<BufferResource>();
	res->name = name;
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = (VkBufferUsageFlagBits)usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	if (memory & Memory::CPU) {
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	}
	auto result = vmaCreateBuffer(_ctx.vmaAllocator, &bufferInfo, &allocInfo, &res->buffer, &res->allocation, nullptr);
	DEBUG_VK(result, "Failed to create buffer!");
	ASSERT(result == VK_SUCCESS, "Failed to create buffer!");
	// LOG_INFO("Created buffer {}", name);

	Buffer buffer = {
		.resource = res,
		.size = size,
		.usage = usage,
		.memory = memory,
	};

	if (usage & BufferUsage::Storage) {
		res->rid = _ctx.availableBufferRID.back(); // TODO test: give RID starting from 0, not from end
		_ctx.availableBufferRID.pop_back();

		VkDescriptorBufferInfo descriptorInfo = {};
		descriptorInfo.buffer = res->buffer;
		descriptorInfo.offset = 0;
		descriptorInfo.range  = size;

		VkWriteDescriptorSet write = {};
		write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet          = _ctx.bindlessDescriptorSet;
		// write.dstSet          = _ctx.descriptorSet;
		write.dstBinding      = BINDING_BUFFER;
		write.dstArrayElement = buffer.RID();
		write.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo     = &descriptorInfo;
		vkUpdateDescriptorSets(_ctx.device, 1, &write, 0, nullptr);
	}

	return buffer;
}

Image CreateImage(const ImageDesc& desc) {
	auto device = _ctx.device;
	auto allocator = _ctx.allocator;

	std::shared_ptr<ImageResource> res = std::make_shared<ImageResource>();

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = desc.width;
	imageInfo.extent.height = desc.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = desc.layers;
	imageInfo.format = (VkFormat)desc.format;
	// tiling defines how the texels lay in memory
	// optimal tiling is implementation dependent for more efficient memory access
	// and linear makes the texels lay in row-major order, possibly with padding on each row
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	// not usable by the GPU, the first transition will discard the texels
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = (VkImageUsageFlags)desc.usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	if (desc.layers == 6) {
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	auto result = vmaCreateImage(_ctx.vmaAllocator, &imageInfo, &allocInfo, &res->image, &res->allocation, nullptr);
	DEBUG_VK(result, "Failed to create image!");
	LOG_INFO("Created image {}", desc.name);

	AspectFlags aspect = Aspect::Color;
	if (desc.format == Format::D24_unorm_S8_uint || desc.format == Format::D32_sfloat) {
		aspect = Aspect::Depth;
	}
	if (desc.format == Format::D24_unorm_S8_uint) {
		aspect |= Aspect::Stencil;
	}

	res->name = desc.name;

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = res->image;
	if (desc.layers == 1) {
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	} else {
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	}
	viewInfo.format = (VkFormat)desc.format;
	viewInfo.subresourceRange.aspectMask = (VkImageAspectFlags)aspect;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = desc.layers;

	// TODO: Create image view only if usage if Sampled or Storage or other fitting
	result = vkCreateImageView(device, &viewInfo, allocator, &res->view);
	DEBUG_VK(result, "Failed to create image view!");
	ASSERT(result == VK_SUCCESS, "Failed to create image view!");

	if (desc.layers > 1) {
		viewInfo.subresourceRange.layerCount = 1;
		res->layersView.resize(desc.layers);
		for (int i = 0; i < desc.layers; i++) {
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.subresourceRange.baseArrayLayer = i;
			result = vkCreateImageView(device, &viewInfo, allocator, &res->layersView[i]);
			DEBUG_VK(result, "Failed to create image view!");
			ASSERT(result == VK_SUCCESS, "Failed to create image view!");
		}
	}

	Image image = {
		.resource = res,
		.width = desc.width,
		.height = desc.height,
		.usage = desc.usage,
		.format = desc.format,
		.layout = ImageLayout::Undefined,
		.aspect = aspect,
		.layers = desc.layers,
	};

	if (desc.usage & ImageUsage::Sampled || desc.usage & ImageUsage::Storage) {
		res->rid = _ctx.availableImageRID.back();
		_ctx.availableImageRID.pop_back();
	}

	if (desc.usage & ImageUsage::Sampled) {
		ImageLayout newLayout = ImageLayout::ShaderRead;
		if (aspect == (Aspect::Depth | Aspect::Stencil)) {
			newLayout = ImageLayout::DepthStencilRead;
		} else if (aspect == Aspect::Depth) {
			newLayout = ImageLayout::DepthReadOnly;
		}
		// res->imguiRIDs.resize(desc.layers);
		// if (desc.layers > 1) {
		//     for (int i = 0; i < desc.layers; i++) {
		//         res->imguiRIDs[i] = ImGui_ImplVulkan_AddTexture(_ctx.genericSampler, res->layersView[i], (VkImageLayout)newLayout);
		//     }
		// } else {
		//     res->imguiRIDs[0] = ImGui_ImplVulkan_AddTexture(_ctx.genericSampler, res->view, (VkImageLayout)newLayout);
		// }

		VkDescriptorImageInfo descriptorInfo = {
			.sampler = _ctx.genericSampler,
			.imageView = res->view,
			.imageLayout = (VkImageLayout)newLayout,
		};
		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = _ctx.bindlessDescriptorSet;
		write.dstBinding = BINDING_TEXTURE;
		write.dstArrayElement = image.RID();
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.descriptorCount = 1;
		write.pImageInfo = &descriptorInfo;
		vkUpdateDescriptorSets(_ctx.device, 1, &write, 0, nullptr);
	}
	if (desc.usage & ImageUsage::Storage) {
		VkDescriptorImageInfo descriptorInfo = {
			.sampler = _ctx.genericSampler,
			.imageView = res->view,
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
		};
		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = _ctx.bindlessDescriptorSet;
		write.dstBinding = BINDING_STORAGE_IMAGE;
		write.dstArrayElement = image.RID();
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.descriptorCount = 1;
		write.pImageInfo = &descriptorInfo;
		vkUpdateDescriptorSets(_ctx.device, 1, &write, 0, nullptr);
	}

	VkDebugUtilsObjectNameInfoEXT name = {
	.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
	.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE,
	.objectHandle = (uint64_t)(VkImage)res->image,
	.pObjectName = desc.name.c_str(),
	};
	_ctx.vkSetDebugUtilsObjectNameEXT(_ctx.device, &name);
	std::string strName = desc.name + "View";
	name = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE_VIEW,
		.objectHandle = (uint64_t)(VkImageView)res->view,
		.pObjectName = strName.c_str(),
	};
	_ctx.vkSetDebugUtilsObjectNameEXT(_ctx.device, &name);

	return image;
}


void Command::Dispatch(const uvec3& groups) {
	vkCmdDispatch(resource->buffer, groups.x, groups.y, groups.z);
}

std::vector<char> ReadBinaryFile(const std::string& path) {
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	// LOG_DEBUG("Reading binary file: " + path);
	ASSERT(file.is_open(), "Failed to open file! " + path);
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

void Context::LoadShaders(Pipeline& pipeline) {
	pipeline.stageBytes.clear();
	for (auto& stage : pipeline.stages) {
		std::filesystem::path binPath = "bin/" + stage.path.filename().string() + ".spv";
		// LOG_DEBUG("Loading shader: " + binPath.string());
		std::filesystem::path& shaderPath = stage.path;
		bool compilationRequired = SHADER_ALWAYS_COMPILE;
		// check if already compiled to .spv
		if (std::filesystem::exists(binPath) && std::filesystem::exists(shaderPath)) {
			auto lastSpvUpdate = std::filesystem::last_write_time(binPath);
			auto lastShaderUpdate = std::filesystem::last_write_time(shaderPath);
			if (lastShaderUpdate > lastSpvUpdate) {
				compilationRequired = true;
			}
		} else {
			compilationRequired = true;
		}	
		if (compilationRequired) {
			pipeline.stageBytes.push_back(std::move(CompileShader(stage.path, stage.entryPoint.c_str())));
		} else {
			pipeline.stageBytes.push_back(std::move(ReadBinaryFile(binPath.string())));
		}
	}
}
// TODO: change for slang and check if compiled fresh spv exists
std::vector<char> Context::CompileShader(const std::filesystem::path& path, const char* entryPoint) {
	char compile_string[1024];
	char inpath[256];
	char outpath[256];
	std::string cwd = std::filesystem::current_path().string();
	sprintf(inpath, "%s", path.string().c_str());
	sprintf(outpath, BIN_PATH"/%s.spv", path.filename().string().c_str()); //TODO: add hash (time) to spv filename
	if (path.extension() == ".slang"){
		sprintf(compile_string, "%s %s -o %s -entry %s -Wno-39001 -Isource/Shaders", SLANGC, inpath, outpath, entryPoint);
	} else{
		sprintf(compile_string, "%s -gVS -V %s -o %s -e %s --target-env spirv1.4 -DGLSL -Isource/Shaders", GLSL_VALIDATOR, inpath, outpath, entryPoint);
	}

	DEBUG_TRACE("[ShaderCompiler] Command: {}", compile_string);
	DEBUG_TRACE("[ShaderCompiler] Output: {}", outpath);
	while(system(compile_string)) {
		LOG_WARN("[ShaderCompiler] Error! Press something to Compile Again");
		std::cin.get();
	}

	return ReadBinaryFile(outpath);
}

Pipeline CreatePipeline(const PipelineDesc& desc) {// External handle
	Pipeline pipeline;
	pipeline.resource = std::make_shared<PipelineResource>();
	pipeline.stages = desc.stages;
	_ctx.LoadShaders(pipeline); // load into pipeline
	_ctx.CreatePipelineImpl(desc, pipeline);
	return pipeline;
}

void Context::CreatePipelineImpl(const PipelineDesc& desc, Pipeline& pipeline) {
	pipeline.point = desc.point; // Graphics or Compute
	pipeline.resource->name = desc.name;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(desc.stages.size());
	std::vector<VkShaderModule> shaderModules(desc.stages.size());
	for (int i = 0; i < desc.stages.size(); i++) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = pipeline.stageBytes[i].size();
		createInfo.pCode = (const uint32_t*)(pipeline.stageBytes[i].data());
		auto result = vkCreateShaderModule(device, &createInfo, allocator, &shaderModules[i]);
		DEBUG_VK(result, "Failed to create shader module!");
		ASSERT(result == VK_SUCCESS, "Failed to create shader module!");
		shaderStages[i] = {};
		shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[i].stage = (VkShaderStageFlagBits)desc.stages[i].stage;
		shaderStages[i].module = shaderModules[i];
		// function to invoke inside the shader module
		// it's possible to combine multiple shaders into a single module
		// using different entry points
		shaderStages[i].pName = pipeline.stages[i].entryPoint.c_str();
		// this allow us to specify values for shader constants
		shaderStages[i].pSpecializationInfo = nullptr;
	}

	std::vector<VkDescriptorSetLayout> layouts;
	layouts.push_back(bindlessDescriptorLayout);
	// layouts.push_back(descriptorSetLayout);

	// TODO: opt, specify each time, but keep compatible(same) for all pipeline layouts
	VkPushConstantRange pushConstant{};
	pushConstant.offset = 0;
	pushConstant.size = physicalProperties.limits.maxPushConstantsSize;
	pushConstant.stageFlags = VK_SHADER_STAGE_ALL;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = layouts.size();
	pipelineLayoutInfo.pSetLayouts = layouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

	auto vkRes = vkCreatePipelineLayout(device, &pipelineLayoutInfo, allocator, &pipeline.resource->layout);
	DEBUG_VK(vkRes, "Failed to create pipeline layout!");
	ASSERT(vkRes == VK_SUCCESS, "Failed to create pipeline layout!");

	if (desc.point == PipelinePoint::Compute) {
		ASSERT(shaderStages.size() == 1, "Compute pipeline only support 1 stage.");

		VkComputePipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.stage = shaderStages[0];
		pipelineInfo.layout = pipeline.resource->layout;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.pNext = VK_NULL_HANDLE;

		vkRes = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, allocator, &pipeline.resource->pipeline);

		DEBUG_VK(vkRes, "Failed to create compute pipeline!");
		ASSERT(vkRes == VK_SUCCESS, "Failed to create compute pipeline!");
	} else {
		// graphics pipeline
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		// fragments beyond near and far planes are clamped to them
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		// line thickness in terms of number of fragments
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = (VkCullModeFlags)desc.cullMode;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.minSampleShading = 0.5f;
		multisampling.pSampleMask = nullptr;

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};

		std::vector<VkVertexInputAttributeDescription> attributeDescs(desc.vertexAttributes.size());
		uint32_t attributeSize = 0;
		for (int i = 0; i < desc.vertexAttributes.size(); i++) {
			attributeDescs[i].binding = 0;
			attributeDescs[i].location = i;
			attributeDescs[i].format = (VkFormat)desc.vertexAttributes[i];
			attributeDescs[i].offset = attributeSize;
			if (desc.vertexAttributes[i] == Format::RG32_sfloat) {
				attributeSize += 2 * sizeof(float);
			} else if (desc.vertexAttributes[i] == Format::RGB32_sfloat) {
				attributeSize += 3 * sizeof(float);
			} else if (desc.vertexAttributes[i] == Format::RGBA32_sfloat) {
				attributeSize += 4 * sizeof(float);
			} else {
				DEBUG_ASSERT(false, "Invalid Vertex Attribute");
			}
		}

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = attributeSize;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)(attributeDescs.size());
		// these points to an array of structs that describe how to load the vertex data
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescs.data();

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		
		// define the type of input of our pipeline
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		if (desc.lineTopology) {
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		} else {
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
		// with this parameter true we can break up lines and triangles in _STRIP topology modes
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineDynamicStateCreateInfo dynamicCreate = {};
		dynamicCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicCreate.pDynamicStates = dynamicStates.data();
		dynamicCreate.dynamicStateCount = dynamicStates.size();

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = nullptr;
		viewportState.scissorCount = 1;
		viewportState.pScissors = nullptr;

		VkPipelineRenderingCreateInfoKHR pipelineRendering{};
		pipelineRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		pipelineRendering.colorAttachmentCount = desc.colorFormats.size();
		pipelineRendering.pColorAttachmentFormats = (VkFormat*)desc.colorFormats.data();
		pipelineRendering.depthAttachmentFormat = desc.useDepth ? (VkFormat)desc.depthFormat : VK_FORMAT_UNDEFINED;
		pipelineRendering.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
		pipelineRendering.viewMask = 0;

		std::vector<VkPipelineColorBlendAttachmentState> blendAttachments(desc.colorFormats.size());
		for (int i = 0; i < desc.colorFormats.size(); i++) {
			blendAttachments[i].colorWriteMask =  VK_COLOR_COMPONENT_R_BIT;
			blendAttachments[i].colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
			blendAttachments[i].colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
			blendAttachments[i].colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
			blendAttachments[i].blendEnable = VK_FALSE;
		}

		VkPipelineColorBlendStateCreateInfo colorBlendState{};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.logicOpEnable = VK_FALSE;
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendState.attachmentCount = blendAttachments.size();
		colorBlendState.pAttachments = blendAttachments.data();
		colorBlendState.blendConstants[0] = 0.0f;
		colorBlendState.blendConstants[1] = 0.0f;
		colorBlendState.blendConstants[2] = 0.0f;
		colorBlendState.blendConstants[3] = 0.0f;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlendState;
		pipelineInfo.pDynamicState = &dynamicCreate;
		pipelineInfo.layout = pipeline.resource->layout;
		pipelineInfo.renderPass = VK_NULL_HANDLE;
		// pipelineInfo.renderPass = SwapChain::GetRenderPass();
		pipelineInfo.subpass = 0;
		// if we were creating this pipeline by deriving it from another
		// we should specify here
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.pNext = &pipelineRendering;

		vkRes = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, allocator, &pipeline.resource->pipeline);
		DEBUG_VK(vkRes, "Failed to create graphics pipeline!");
	}

	for (int i = 0; i < shaderModules.size(); i++) {
		vkDestroyShaderModule(device, shaderModules[i], allocator);
	}
}

Command::Command() {
	resource = std::make_shared<CommandResource>();
}

void Command::BeginRendering(const std::vector<Image>& colorAttachs, Image depthAttach, uint32_t layerCount, vec4 viewport, ivec4 scissor) {

	ivec2 offset(0, 0);
	uvec2 extent(0, 0);

	if (colorAttachs.size() > 0) {
		extent.x = colorAttachs[0].width;
		extent.y = colorAttachs[0].height;
	} else if (depthAttach.resource) {
		extent.x = depthAttach.width;
		extent.y = depthAttach.height;
	}

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	renderingInfo.viewMask = 0;
	renderingInfo.layerCount = layerCount;
	renderingInfo.renderArea.extent = { uint32_t(extent.x), uint32_t(extent.y)};
	renderingInfo.renderArea.offset = { offset.x, offset.y };
	renderingInfo.flags = 0;

	std::vector<VkRenderingAttachmentInfoKHR> colorAttachInfos(colorAttachs.size());
	VkRenderingAttachmentInfoKHR depthAttachInfo;
	for (int i = 0; i < colorAttachs.size(); i++) {
		colorAttachInfos[i] = {};
		colorAttachInfos[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		colorAttachInfos[i].imageView = colorAttachs[i].resource->view;
		colorAttachInfos[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachInfos[i].resolveMode = VK_RESOLVE_MODE_NONE;
		// colorAttachInfos[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachInfos[i].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachInfos[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachInfos[i].clearValue.color = { 0.1f, 0.4f, 0.1f, 0.0f };
	}

	renderingInfo.colorAttachmentCount = colorAttachInfos.size();
	renderingInfo.pColorAttachments = colorAttachInfos.data();
	renderingInfo.pDepthAttachment = nullptr;
	renderingInfo.pStencilAttachment = nullptr;

	if (depthAttach.resource) {
		depthAttachInfo = {};
		depthAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		depthAttachInfo.imageView = depthAttach.resource->view;
		depthAttachInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachInfo.resolveMode = VK_RESOLVE_MODE_NONE;
		depthAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachInfo.clearValue.depthStencil = { 1.0f, 0 };
		renderingInfo.pDepthAttachment = &depthAttachInfo;
	}
	
	if (viewport == vec4(0.0f)) {
		viewport = { 0.0f, 0.0f, static_cast<float>(extent.x), static_cast<float>(extent.y) };
	}

	if (scissor == ivec4(0)) {
		scissor = { offset.x, offset.y, static_cast<int>(extent.x), static_cast<int>(extent.y) };
	}

	VkViewport _viewport = {};
	_viewport.x = viewport.x;
	_viewport.y = viewport.y;
	_viewport.width = viewport.z;
	_viewport.height = viewport.w;
	_viewport.minDepth = 0.0f;
	_viewport.maxDepth = 1.0f;

	VkRect2D _scissor = {};
	_scissor.offset = { scissor.x, scissor.y };
	_scissor.extent.width = scissor.z;
	_scissor.extent.height = scissor.w;
	vkCmdSetViewport(resource->buffer, 0, 1, &_viewport);
	vkCmdSetScissor(resource->buffer, 0, 1, &_scissor);

	vkCmdBeginRendering(resource->buffer, &renderingInfo);
}

void Command::EndRendering() {
	vkCmdEndRendering(resource->buffer);
}

// Acquire + CmdBarrier + CmdBeginRendering
// void Command::BeginPresent() {
// 	vkw::AcquireImage();
// 	vkw::Barrier(_ctx.GetCurrentSwapChainImage(), vkw::ImageLayoutColorAttachment);
// 	vkw::BeginRendering({ _ctx.GetCurrentSwapChainImage() }, {});
// }
// CmdEndRendering + CmdBarrier
// void Command::EndPresent() {
// 	vkw::EndRendering();
// 	vkw::Barrier(_ctx.GetCurrentSwapChainImage(), vkw::ImageLayoutPresent);
// }

void Command::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
	// DEBUG_TRACE("CmdDraw({},{},{},{})", vertexCount, instanceCount, firstVertex, firstInstance);
	vkCmdDraw(resource->buffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void Command::DrawMesh(Buffer& vertexBuffer, Buffer& indexBuffer, uint32_t indexCount) {
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(resource->buffer, 0, 1, &vertexBuffer.resource->buffer, offsets);
	vkCmdBindIndexBuffer(resource->buffer, indexBuffer.resource->buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(resource->buffer, indexCount, 1, 0, 0, 0);
}

void Command::BindVertexBuffer(Buffer& vertexBuffer) {
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(resource->buffer, 0, 1, &vertexBuffer.resource->buffer, offsets);
}

void Command::DrawLineStrip(const Buffer& pointsBuffer, uint32_t firstPoint, uint32_t pointCount, float thickness) {
	VkDeviceSize offsets[] = { 0 };
	vkCmdSetLineWidth(resource->buffer, thickness);
	vkCmdBindVertexBuffers(resource->buffer, 0, 1, &pointsBuffer.resource->buffer, offsets);
	vkCmdDraw(resource->buffer, pointCount, 1, firstPoint, 0);
}

// void Command::DrawPassThrough() {
//     VkDeviceSize offsets[] = { 0 };
//     vkCmdBindVertexBuffers(resource->buffer, 0, 1, &_ctx.dummyVertexBuffer.resource->buffer, offsets);
//     vkCmdDraw(resource->buffer, 6, 1, 0, 0);
// }

// Command::void CmdDrawImGui(ImDrawData* data) {
//     ImGui_ImplVulkan_RenderDrawData(data, _ctx.GetCurrentCommandResources().buffer);
// }


// int CmdBeginTimeStamp(const std::string& name) {
//     DEBUG_ASSERT(_ctx.currentQueue != Queue::Transfer, "Time Stamp not supported in Transfer queue");
//     int id = resource->timeStamps.size();
//     if (id >= _ctx.timeStampPerPool - 1) {
//         LOG_WARN("Maximum number of time stamp per pool exceeded. Ignoring Time stamp {}", name.c_str());
//         return -1;
//     }
//     vkCmdWriteTimestamp(resource->buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, resource->queryPool, id);
//     resource->timeStamps.push_back(0);
//     resource->timeStamps.push_back(0);
//     resource->timeStampNames.push_back(name);
//     return id;
// }

// Command::void CmdEndTimeStamp(int timeStampIndex) {
//     if (timeStampIndex >= 0 && timeStampIndex < _ctx.timeStampPerPool - 1) {
//         vkCmdWriteTimestamp(resource->buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, resource->queryPool, timeStampIndex + 1);
//     }
// }

void Command::BindPipeline(Pipeline& pipeline) {
	vkCmdBindPipeline(resource->buffer, (VkPipelineBindPoint)pipeline.point, pipeline.resource->pipeline);
	// TODO: bind only if not compatible for used descriptor sets or push constant range
	// ref: https://registry.khronos.org/vulkan/specs/1.2-extensions/html/vkspec.html#descriptorsets-compatibility
	vkCmdBindDescriptorSets(resource->buffer, (VkPipelineBindPoint)pipeline.point, pipeline.resource->layout, 0, 1, &_ctx.bindlessDescriptorSet, 0, nullptr);
	// vkCmdBindDescriptorSets(resource->buffer, (VkPipelineBindPoint)pipeline.point, pipeline.resource->layout, 0, 1, &_ctx.descriptorSet, 0, nullptr);

	resource->currentPipeline = pipeline.resource.get();
}

void Command::PushConstants(void* data, uint32_t size) {
	vkCmdPushConstants(resource->buffer, resource->currentPipeline->layout, VK_SHADER_STAGE_ALL, 0, size, data);
}

// void BeginImGui() {
//     ImGui_ImplVulkan_NewFrame();
//     ImGui_ImplGlfw_NewFrame();
// }

// for swapchain

Command& GetCommandBuffer(GLFWwindow* window) {
	auto& swapChain = _ctx.swapChains[window];
	return swapChain.GetCommandBuffer();
}

Command& GetCommandBuffer (Queue queue) {
	return *_ctx.queues[queue]->commands;
}

// vkWaitForFences + vkResetFences +
// vkResetCommandPool + vkBeginCommandBuffer
void Command::BeginCommandBuffer() {
	// ASSERT(_ctx.currentQueue == Queue::Count, "Already recording a command buffer");
	// _ctx.currentQueue = queue;
	vkWaitForFences(_ctx.device, 1, &resource->fence, VK_TRUE, UINT64_MAX);
	vkResetFences(_ctx.device, 1, &resource->fence);

	// if (resource->timeStamps.size() > 0) {
	//     vkGetQueryPoolResults(_ctx.device, resource->queryPool, 0, resource->timeStamps.size(), resource->timeStamps.size() * sizeof(uint64_t), resource->timeStamps.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);
	//     for (int i = 0; i < resource->timeStampNames.size(); i++) {
	//         const uint64_t begin = resource->timeStamps[2 * i];
	//         const uint64_t end = resource->timeStamps[2 * i + 1];
	//         _ctx.timeStampTable[resource->timeStampNames[i]] = float(end - begin) * _ctx.physicalProperties.limits.timestampPeriod / 1000000.0f;
	//     }
	//     resource->timeStamps.clear();
	//     resource->timeStampNames.clear();
	// }

	// ?VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
	vkResetCommandPool(_ctx.device, resource->pool, 0); // TODO: check if this is needed
	resource->stagingOffset = 0;
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(resource->buffer, &beginInfo);

	// if (queue != Queue::Transfer) {
	//     vkCmdResetQueryPool(resource->buffer, resource->queryPool, 0, _ctx.timeStampPerPool);
	// }
}

void Command::EndCommandBuffer() {
	vkEndCommandBuffer(resource->buffer); 
	// _ctx.currentQueue = vkw::Queue::Count;
	resource->currentPipeline = nullptr;
}

void Command::WaitQueue() {
	auto res = vkQueueWaitIdle(resource->queue->queue);
	DEBUG_VK(res, "Failed to wait idle command buffer");
}

void Command::QueueSubmit(const SubmitInfo& submitInfo) {

	VkSemaphoreSubmitInfo waitInfo { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
	waitInfo.semaphore = (VkSemaphore)submitInfo.waitSemaphore;
	waitInfo.stageMask = (VkPipelineStageFlags2)submitInfo.waitStages;
	// waitInfo.value = 1; // Only for timeline semaphores

	VkSemaphoreSubmitInfo signalInfo { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
	signalInfo.semaphore = (VkSemaphore)submitInfo.signalSemaphore;
	signalInfo.stageMask = (VkPipelineStageFlags2)submitInfo.signalStages;

	VkCommandBufferSubmitInfo cmdInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
	cmdInfo.commandBuffer = resource->buffer;

	VkSubmitInfo2 info{};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	info.waitSemaphoreInfoCount = submitInfo.waitSemaphore ? 1 : 0; // TODO pass by parameter
	info.pWaitSemaphoreInfos = &waitInfo;
	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos = &cmdInfo;
	info.signalSemaphoreInfoCount = submitInfo.signalSemaphore ? 1 : 0;
	info.pSignalSemaphoreInfos = &signalInfo;

	auto res = vkQueueSubmit2(resource->queue->queue, 1, &info, resource->fence);
	DEBUG_VK(res, "Failed to submit command buffer");
}

void WaitQueue(Queue queue) {
	// todo: wait on fence
	auto res = vkQueueWaitIdle(_ctx.queues[queue]->queue);
	DEBUG_VK(res, "Failed to wait idle command buffer");
}


void WaitIdle() {
	auto res = vkDeviceWaitIdle(_ctx.device);
	DEBUG_VK(res, "Failed to wait idle command buffer");
}



namespace{ // utils
// https://github.com/charles-lunarg/vk-bootstrap/blob/main/src/VkBootstrap.cpp
bool check_extension_supported(std::vector<VkExtensionProperties> const& available_extensions, const char* extension_name) {
	if (!extension_name) return false;
	for (const auto& extension_properties : available_extensions) {
		if (strcmp(extension_name, extension_properties.extensionName) == 0) {
			return true;
		}
	}
	return false;
}

bool check_extensions_supported(
	std::vector<VkExtensionProperties> const& available_extensions, std::vector<const char*> const& extension_names) {
	bool all_found = true;
	for (const auto& extension_name : extension_names) {
		bool found = check_extension_supported(available_extensions, extension_name);
		if (!found) all_found = false;
	}
	return all_found;
}

template <typename T> void setup_pNext_chain(T& structure, std::vector<VkBaseOutStructure*> const& structs) {
	structure.pNext = nullptr;
	if (structs.size() <= 0) return;
	for (size_t i = 0; i < structs.size() - 1; i++) {
		structs.at(i)->pNext = structs.at(i + 1);
	}
	structure.pNext = structs.at(0);
}

}

namespace { // vulkan debug callbacks
VkResult CreateDebugUtilsMessengerEXT (
	VkInstance                                instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks*              pAllocator,
	VkDebugUtilsMessengerEXT*                 pDebugMessenger) {
	// search for the requested function and return null if cannot find
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr (
		instance, 
		"vkCreateDebugUtilsMessengerEXT"
	);
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT (
	VkInstance                   instance,
	VkDebugUtilsMessengerEXT     debugMessenger,
	const VkAllocationCallbacks* pAllocator) {
	// search for the requested function and return null if cannot find
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr (
		instance,
		"vkDestroyDebugUtilsMessengerEXT"
	);
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsCallback (
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void*                                       pUserData) {
	// log the message
	// here we can set a minimum severity to log the message
	// if (messageSeverity > VK_DEBUG_UTILS...)
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) { LOG_TRACE("[Validation Layer] {} ", pCallbackData->pMessage); }
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)    { LOG_INFO ("[Validation Layer] {} ", pCallbackData->pMessage); }
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) { LOG_WARN ("[Validation Layer] {} ", pCallbackData->pMessage); }
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)   { LOG_ERROR("[Validation Layer] {} ", pCallbackData->pMessage); }

	return VK_FALSE;	
}
// Do-nothing in case validation layers are not enabled
VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectNameEXT(VkDevice device, VkDebugUtilsObjectNameInfoEXT* pNameInfo) {
	return VK_SUCCESS;
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	// createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
	createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
	createInfo.pfnUserCallback = DebugUtilsCallback;
	createInfo.pUserData = nullptr;
}

// Debug Report
VkResult CreateDebugReportCallbackEXT (
	VkInstance                                instance,
	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks*              pAllocator,
	VkDebugReportCallbackEXT*                 pDebugReport) {
	// search for the requested function and return null if cannot find
	auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr (
		instance, 
		"vkCreateDebugReportCallbackEXT"
	);
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugReport);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT (
	VkInstance                   instance,
	VkDebugReportCallbackEXT     debugReport,
	const VkAllocationCallbacks* pAllocator) {
	// search for the requested function and return null if cannot find
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr (
		instance,
		"vkDestroyDebugReportCallbackEXT"
	);
	if (func != nullptr) {
		func(instance, debugReport, pAllocator);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType,
	uint64_t object, 
	size_t location, 
	int32_t messageCode,
	const char* pLayerPrefix,
	const char* pMessage, 
	void* pUserData)
{
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)         { LOG_INFO ("[Debug Report] {0}", pMessage) };
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)             { LOG_WARN ("[Debug Report] {0}", pMessage) };
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) { LOG_WARN ("[Debug Report Performance] {0}", pMessage) };
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)               { LOG_ERROR("[Debug Report] {0}", pMessage) };
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)               { LOG_TRACE("[Debug Report] {0}", pMessage) };

	return VK_FALSE;
}



void PopulateDebugReportCreateInfo(VkDebugReportCallbackCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags |= VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
	createInfo.flags |= VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.flags |= VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	createInfo.flags |= VK_DEBUG_REPORT_ERROR_BIT_EXT;
	createInfo.flags |= VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	createInfo.pfnCallback = DebugReportCallback;
}

} // vulkan debug callbacks

void Context::CreateInstance(GLFWwindow* glfwWindow){
	// optional data, provides useful info to the driver
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = applicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.pEngineName = engineName.c_str();
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// get api version
	vkEnumerateInstanceVersion(&apiVersion);

	// Validation layers
	if (enableValidationLayers) {
		// get all available layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		layers.resize(layerCount);
		activeLayers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		// active default khronos validation layer
		const char* validation_layer_name = "VK_LAYER_KHRONOS_validation";
		bool khronosAvailable = false;
		for (size_t i = 0; i < layerCount; i++) {
			activeLayers[i] = false;
			if (strcmp(validation_layer_name, layers[i].layerName) == 0) {
				activeLayers[i] = true;
				khronosAvailable = true;
				break;
			}
		}
		if (!khronosAvailable) {LOG_ERROR("Default validation layer not available!");}

		for (size_t i = 0; i < layerCount; i++) { 
			if (activeLayers[i]) {
				activeLayersNames.push_back(layers[i].layerName);
			}
		}
	}
	
	std::vector<const char*> requiredInstanceExtensions;
	uint32_t glfwExtensionCount = 0;
	if (presentRequested) {
		// get all extensions required by glfw
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		requiredInstanceExtensions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}

	// Extensions
	if (enableValidationLayers) {
		requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		if (enableDebugReport) {
			requiredInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
	}

	// get all available extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 0);
	instanceExtensions.resize(extensionCount);
	activeExtensions.resize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

	// set to active all extensions that we enabled //TODO: Rewrite loop
	for (size_t i = 0; i < requiredInstanceExtensions.size(); i++) {
		for (size_t j = 0; j < extensionCount; j++) {
			if (strcmp(requiredInstanceExtensions[i], instanceExtensions[j].extensionName) == 0) {
				activeExtensions[j] = true;
				break;
			}
		}
	}

	// Enable validation features if available
	bool validation_features = false;
	if (enableValidationLayers){
		for (size_t i = 0; i < extensionCount; i++) {
			if (strcmp(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME, instanceExtensions[i].extensionName) == 0) {
				validation_features = true;
				activeExtensions[i] = true;
				LOG_INFO("{} is available, enabling it", VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
				break;
			}
		}
	}

	VkValidationFeaturesEXT validation_features_info = {VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT};
	if (validation_features)
	{
		static const VkValidationFeatureEnableEXT enable_features[3] = {
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
			VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
		};
		validation_features_info.enabledValidationFeatureCount = 3;
		validation_features_info.pEnabledValidationFeatures    = enable_features;
		validation_features_info.pNext                         = createInfo.pNext;
		createInfo.pNext                                       = &validation_features_info;
	}

	// get the name of all extensions that we enabled
	activeExtensionsNames.clear();
	for (size_t i = 0; i < extensionCount; i++) {
		if (activeExtensions[i]) {
			activeExtensionsNames.push_back(instanceExtensions[i].extensionName);
		}
	}

	// get and set all required extensions
	createInfo.enabledExtensionCount = static_cast<uint32_t>(activeExtensionsNames.size());
	createInfo.ppEnabledExtensionNames = activeExtensionsNames.data();

	

	// which validation layers we need
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(activeLayersNames.size());
		createInfo.ppEnabledLayerNames = activeLayersNames.data();

		// we need to set up a separate logger just for the instance creation/destruction
		// because our "default" logger is created after
		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		debugCreateInfo.pNext = createInfo.pNext;
		createInfo.pNext      = &debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// Instance creation
	auto res = vkCreateInstance(&createInfo, allocator, &instance);
	// LOG_DEBUG("Enabled extensions count: {}", createInfo.enabledExtensionCount);
	// if (createInfo.enabledExtensionCount > 0)
	// 	LOG_DEBUG("Enabled extensions: {}", createInfo.ppEnabledExtensionNames[0]);
	// auto res = VK_SUCCESS;
	DEBUG_VK(res, "Failed to create Vulkan instance!");
	DEBUG_TRACE("Created instance.");
	
	// Debug Utils
	if (enableValidationLayers) {
		VkDebugUtilsMessengerCreateInfoEXT messengerInfo;
		PopulateDebugMessengerCreateInfo(messengerInfo);
		res = CreateDebugUtilsMessengerEXT(instance, &messengerInfo, allocator, &debugMessenger);
		DEBUG_VK(res, "Failed to set up debug messenger!");
		DEBUG_TRACE("Created debug messenger.");
	}

	// Debug Report
	if (enableValidationLayers && enableDebugReport) {
		VkDebugReportCallbackCreateInfoEXT debugReportInfo;
		PopulateDebugReportCreateInfo(debugReportInfo);
		// Create the callback handle
		res = CreateDebugReportCallbackEXT(instance, &debugReportInfo, nullptr, &debugReport);
		// DEBUG_VK(res, "Failed to set up debug report callback!");
		DEBUG_TRACE("Created debug report callback.");
	}

	if (_ctx.presentRequested) {
		res = glfwCreateWindowSurface(instance, glfwWindow, allocator, &_dummySurface);
		DEBUG_VK(res, "Failed to create window surface!");
		DEBUG_TRACE("Created surface.");
	}
	LOG_INFO("Created VulkanInstance.");
}

void Context::DestroyInstance() {
	activeLayersNames.clear();
	activeExtensionsNames.clear();
	if (debugMessenger) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, allocator);
		DEBUG_TRACE("Destroyed debug messenger.");
		debugMessenger = nullptr;
	}
	if (debugReport) {
		DestroyDebugReportCallbackEXT(instance, debugReport, allocator);
		DEBUG_TRACE("Destroyed debug report callback.");
		debugReport = nullptr;
	}
	vkDestroyInstance(instance, allocator);
	DEBUG_TRACE("Destroyed instance.");
	LOG_INFO("Destroyed VulkanInstance");
}

void Context::CreatePhysicalDevice() {
	// get all devices with Vulkan support
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(instance, &count, nullptr);
	ASSERT(count != 0, "no GPUs with Vulkan support!");
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(instance, &count, devices.data());
	DEBUG_TRACE("Found {0} physical device(s).", count);

	for (const auto& device : devices) {
		// get all available extensions
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		availableExtensions.resize(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		// get all available families
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
		availableFamilies.resize(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, availableFamilies.data());

		bool graphicsAvailable = false; // TODO: if (presentRequested)
		bool computeAvailable = false;
		bool transferAvailable = false;
		VkBool32 presentAvailable = false;

		// select the family for each type of queue that we want
		for (int i = 0; i < familyCount; i++) {
			auto& family = availableFamilies[i];
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) graphicsAvailable = true;
			if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) computeAvailable = true;
			if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) transferAvailable = true;
			if (presentRequested){
				if (presentAvailable == false){
					VkBool32 present = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _dummySurface, &present);
					if (present) presentAvailable = true;
				}
			}
		}
		
		bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		indexingFeatures.sType	= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
		indexingFeatures.pNext = &bufferDeviceAddressFeatures;
		physicalFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		physicalFeatures2.pNext = &indexingFeatures;
		vkGetPhysicalDeviceFeatures2(device, &physicalFeatures2);
		vkGetPhysicalDeviceProperties(device, &physicalProperties);
		vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);
		// imageFormatProperties2.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
		// imageFormatInfo2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
		// vkGetPhysicalDeviceImageFormatProperties2(device, &imageFormatInfo2, &imageFormatProperties2);

		VkSampleCountFlags counts = physicalProperties.limits.framebufferColorSampleCounts;
		counts &= physicalProperties.limits.framebufferDepthSampleCounts;

		// get max number of samples
		maxSamples = VK_SAMPLE_COUNT_1_BIT;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { maxSamples = VK_SAMPLE_COUNT_64_BIT; }
		else if (counts & VK_SAMPLE_COUNT_32_BIT) { maxSamples = VK_SAMPLE_COUNT_32_BIT; }
		else if (counts & VK_SAMPLE_COUNT_16_BIT) { maxSamples = VK_SAMPLE_COUNT_16_BIT; }
		else if (counts & VK_SAMPLE_COUNT_8_BIT) { maxSamples = VK_SAMPLE_COUNT_8_BIT; }
		else if (counts & VK_SAMPLE_COUNT_4_BIT) { maxSamples = VK_SAMPLE_COUNT_4_BIT; }
		else if (counts & VK_SAMPLE_COUNT_2_BIT) { maxSamples = VK_SAMPLE_COUNT_2_BIT; }

		// check if all required extensions are available
		if (presentRequested) requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		std::set<std::string_view> required(requiredExtensions.begin(), requiredExtensions.end());
		for (const auto& extension : availableExtensions) {
			required.erase(extension.extensionName);
		}
		
		// check if all required queues are supported
		bool suitable = required.empty();
		if (!suitable) LOG_ERROR("Required extensions not available: {0}", required.begin()->data());
		suitable &= (graphicsAvailable && computeAvailable && transferAvailable);
		if (presentRequested) {
			suitable &= presentAvailable;
		}
		if (!suitable) LOG_ERROR("Required queue not available");
		// suitable &= graphicsFamily != -1;
		if (suitable) {
			physicalDevice = device;
			break;
		}
	}
	
	ASSERT(physicalDevice != VK_NULL_HANDLE, "no device with Vulkan support!");
	DEBUG_TRACE("Created physical device: {0}", physicalProperties.deviceName);
	if (numSamples > maxSamples) {
		numSamples = maxSamples;
	}
}

void Context::CreateDevice() {
	uint32_t familyCount = availableFamilies.size();

	std::unordered_map<uint32_t, uint32_t> numActiveQueuesInFamilies; // <queueFamilyIndex, queueCount>
	numActiveQueuesInFamilies.reserve(familyCount);
	auto find_first = [&](VkQueueFlags desired_flags) -> std::pair<uint32_t, uint32_t> {
		for (uint32_t i = 0; i < familyCount; i++) {
			auto& family = availableFamilies[i];
			if ((family.queueFlags & desired_flags) == desired_flags){
				if (presentRequested) {
					if (desired_flags & VK_QUEUE_GRAPHICS_BIT) { // TODO: move to separate function and split queues
						VkBool32 present = false;
						vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _dummySurface, &present);
						if (!present) continue;
					}
				}
				// Set active queue count in this family to at least 1
				if (numActiveQueuesInFamilies[i] < 1){
					numActiveQueuesInFamilies[i] = 1;
				};
				// Each family has at least one queue
				return {i, 0}; // return queue with index 0
			}
		}
		LOG_ERROR("No queue with flags {0} found", desired_flags);
		DEBUG_ASSERT(false, "No queue found"); //todo:remove after debugging
		return {~0U, 0};
	};

	auto find_separate = [&](VkQueueFlags desired_flags) -> std::tuple<uint32_t, uint32_t, bool> {
		for (uint32_t i = 0; i < familyCount; i++) {
			auto& family = availableFamilies[i];
			// Check if all queues in family are taken
			if (numActiveQueuesInFamilies[i] >= family.queueCount) continue;
			if ((family.queueFlags & desired_flags) == desired_flags){
				return {i, numActiveQueuesInFamilies[i]++, true};
			}
		}
		LOG_WARN("Separate queue with flags {0} not found", desired_flags);
		auto [familyIndex, indexInFamily] = find_first(desired_flags);
		return {familyIndex, indexInFamily, false};
	};

	uint32_t maxQueuesInFamily = std::max_element(availableFamilies.begin(), availableFamilies.end(), [](const auto& a, const auto& b) {
		return a.queueCount < b.queueCount;
	}).base()->queueCount;
	
	// requestSeparate[Queue::Transfer] = true;

	for (uint32_t i = 0; i < Queue::Count; i++) {
		uint32_t queueFamily, indexInFamily, key;
		bool is_separate;
		if (requestSeparate[i]) {
			std::tie(queueFamily, indexInFamily, is_separate) = find_separate((VkQueueFlagBits)i);
		} else {
			std::tie(queueFamily, indexInFamily) = find_first((VkQueueFlagBits)i);
		}
		key = queueFamily*maxQueuesInFamily + indexInFamily;
		uniqueQueues[key].family = queueFamily;
		uniqueQueues[key].indexInFamily = indexInFamily;
		queues[i] = &uniqueQueues[key];
	}
	
	// priority for each type of queue (1.0f for all)
	std::vector<float> priorities(uniqueQueues.size(), 1.0f);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto family: numActiveQueuesInFamilies) {
		if (family.second == uint32_t{}) continue;
		VkDeviceQueueCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = family.first;
		createInfo.queueCount = family.second;
		createInfo.pQueuePriorities = &priorities[0];
		queueCreateInfos.push_back(createInfo);
	}

	// DEBUG_TRACE("Queue[Graphics]: [{0}][{1}]", queues[Queue::Graphics]->family, queues[Queue::Graphics]->indexInFamily);
	// DEBUG_TRACE("Queue[Compute ]: [{0}][{1}]", queues[Queue::Compute ]->family, queues[Queue::Compute ]->indexInFamily);
	// DEBUG_TRACE("Queue[Transfer]: [{0}][{1}]", queues[Queue::Transfer]->family, queues[Queue::Transfer]->indexInFamily);
	// DEBUG_TRACE("numActiveQueuesInFamilies size: {0}", numActiveQueuesInFamilies.size());
	// DEBUG_TRACE("queues in family[0]: {0}", numActiveQueuesInFamilies[0]);

	auto supportedFeatures = physicalFeatures2.features;
	
	// logical device features
	VkPhysicalDeviceFeatures2 features2 = {};
	features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	// features2.features.geometryShader = VK_TRUE;
	// if (supportedFeatures.logicOp)           { features2.features.logicOp           = VK_TRUE; }
	// if (supportedFeatures.samplerAnisotropy) { features2.features.samplerAnisotropy = VK_TRUE; }
	// if (supportedFeatures.sampleRateShading) { features2.features.sampleRateShading = VK_TRUE; }
	// if (supportedFeatures.fillModeNonSolid)  { features2.features.fillModeNonSolid  = VK_TRUE; }
	// if (supportedFeatures.wideLines)         { features2.features.wideLines         = VK_TRUE; }
	// if (supportedFeatures.depthClamp)        { features2.features.depthClamp        = VK_TRUE; }
	// if (supportedFeatures.multiViewport)     { features2.features.multiViewport     = VK_TRUE; }


	// DELETE LATER (already checked in CreatePhysicalDevice)
	auto requiredExtensions = _ctx.requiredExtensions;
	auto allExtensions = _ctx.availableExtensions;
	for (auto req : requiredExtensions) {
		bool available = false;
		for (size_t i = 0; i < allExtensions.size(); i++) {
			if (strcmp(allExtensions[i].extensionName, req) == 0) { 
				available = true; 
				break;
			}
		}
		if(!available) {
			LOG_ERROR("Required extension {0} not available!", req);
		}
	}

	// ask for features if available
	// ref: https://dev.to/gasim/implementing-bindless-design-in-vulkan-34no
	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
	descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	descriptorIndexingFeatures.runtimeDescriptorArray = indexingFeatures.runtimeDescriptorArray;
	descriptorIndexingFeatures.descriptorBindingPartiallyBound = indexingFeatures.descriptorBindingPartiallyBound;
	descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = indexingFeatures.shaderSampledImageArrayNonUniformIndexing;
	descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing = indexingFeatures.shaderUniformBufferArrayNonUniformIndexing;
	descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = indexingFeatures.shaderStorageBufferArrayNonUniformIndexing;
	descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = indexingFeatures.descriptorBindingSampledImageUpdateAfterBind;
	descriptorIndexingFeatures.descriptorBindingStorageImageUpdateAfterBind = indexingFeatures.descriptorBindingStorageImageUpdateAfterBind;

	VkPhysicalDeviceBufferDeviceAddressFeatures addresFeatures{};
	addresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	addresFeatures.bufferDeviceAddress = bufferDeviceAddressFeatures.bufferDeviceAddress;
	addresFeatures.pNext = &descriptorIndexingFeatures;

	// VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
	// rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	// rayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
	// rayTracingPipelineFeatures.pNext = &addresFeatures;

	// VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};
	// accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	// accelerationStructureFeatures.accelerationStructure = VK_TRUE;
	// accelerationStructureFeatures.descriptorBindingAccelerationStructureUpdateAfterBind = VK_TRUE;
	// accelerationStructureFeatures.accelerationStructureCaptureReplay = VK_TRUE;
	// accelerationStructureFeatures.pNext = &rayTracingPipelineFeatures;

	// VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{};
	// rayQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
	// rayQueryFeatures.rayQuery = VK_TRUE;
	// // rayQueryFeatures.pNext = &accelerationStructureFeatures;
	// rayQueryFeatures.pNext = &addresFeatures;

	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
	dynamicRenderingFeatures.pNext = &addresFeatures;

	VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features{};
	sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
	sync2Features.synchronization2 = VK_TRUE;
	sync2Features.pNext = &dynamicRenderingFeatures;

	// VkPhysicalDeviceShaderAtomicFloatFeaturesEXT atomicFeatures{};
	// atomicFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	// atomicFeatures.shaderBufferFloat32AtomicAdd = VK_TRUE;
	// atomicFeatures.pNext = &sync2Features;

	features2.pNext = &sync2Features;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	// createInfo.queueCreateInfoCount = 1;
	// createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	// createInfo.pEnabledFeatures; // !Should be NULL if pNext is used
	createInfo.pNext = &features2; // feature chain


	// specify the required layers to the device 
	if (_ctx.enableValidationLayers) {
		auto& layers = activeLayersNames;
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	auto res = vkCreateDevice(physicalDevice, &createInfo, allocator, &device);
	DEBUG_VK(res, "Failed to create logical device!");
	ASSERT(res == VK_SUCCESS, "Failed to create logical device!");
	DEBUG_TRACE("Created logical device");

	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	// VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT 
	allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	allocatorCreateInfo.physicalDevice = physicalDevice;
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.instance = instance;
	allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
	vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator);

	for (auto& [key, q]: uniqueQueues) {
		vkGetDeviceQueue(device, q.family, q.indexInFamily, &q.queue);
	}

	genericSampler = CreateSampler(device, 1.0);
	if (enableValidationLayers) {
		vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
	} else {
		vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)SetDebugUtilsObjectNameEXT;
	}
	// vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR");
	// vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR");
	// vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR");
	// vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR");
	// vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR");

	// VkDescriptorPoolSize imguiPoolSizes[]    = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000} };
	// VkDescriptorPoolCreateInfo imguiPoolInfo{};
	// imguiPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	// imguiPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	// imguiPoolInfo.maxSets = (uint32_t)(1024);
	// imguiPoolInfo.poolSizeCount = sizeof(imguiPoolSizes)/sizeof(VkDescriptorPoolSize);
	// imguiPoolInfo.pPoolSizes = imguiPoolSizes;

	// VkResult result = vkCreateDescriptorPool(device, &imguiPoolInfo, allocator, &imguiDescriptorPool);
	// DEBUG_VK(result, "Failed to create imgui descriptor pool!");
	
	if (_dummySurface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, _dummySurface, nullptr);
		_dummySurface = VK_NULL_HANDLE;
	}
}

void Context::DestroyDevice() {
	// dummyVertexBuffer = {};
	currentPipeline = {};
	// asScratchBuffer = {};
	// vkDestroyDescriptorPool(device, imguiDescriptorPool, allocator);
	vmaDestroyAllocator(vmaAllocator);
	vkDestroySampler(device, genericSampler, allocator);
	vkDestroyDevice(device, allocator);
	DEBUG_TRACE("Destroyed logical device");
	device = VK_NULL_HANDLE;
}


void SwapChain::CreateSurfaceFormats() {

	// get capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_ctx.physicalDevice, surface, &surfaceCapabilities);

	// get surface formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_ctx.physicalDevice, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		availableSurfaceFormats.clear();
		availableSurfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_ctx.physicalDevice, surface, &formatCount, availableSurfaceFormats.data());
	}

	// get present modes
	uint32_t modeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_ctx.physicalDevice, surface, &modeCount, nullptr);
	if (modeCount != 0) {
		availablePresentModes.clear();
		availablePresentModes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_ctx.physicalDevice, surface, &modeCount, availablePresentModes.data());
	}

	// set this device as not suitable if no surface formats or present modes available
	bool suitable = (modeCount > 0);
	suitable &= (formatCount > 0);

	if (!suitable) {
		LOG_ERROR("selected device invalidated after surface update.");
	}
}

void SwapChain::Create(GLFWwindow* window, uint32_t width, uint32_t height, bool is_recreation) {
	this->device = _ctx.device;
	this->window = window;
	if (!is_recreation) {
		VkResult res = glfwCreateWindowSurface(_ctx.instance, window, _ctx.allocator, &surface);
		DEBUG_VK(res, "Failed to create window surface!");
		DEBUG_TRACE("Created surface.");
	}
	
	CreateSurfaceFormats();

	// create swapchain
	{
		const auto& capabilities = surfaceCapabilities;
		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(availableSurfaceFormats);
		colorFormat = surfaceFormat.format;
		colorSpace = surfaceFormat.colorSpace;
		presentMode = ChoosePresentMode(availablePresentModes);
		extent = ChooseExtent(capabilities, width, height);

		// acquire additional images to prevent waiting for driver's internal operations
		uint32_t imageCount = framesInFlight + additionalImages;
		
		if (imageCount < capabilities.minImageCount) {
			LOG_WARN("Querying less images {} than the necessary: {}", imageCount, capabilities.minImageCount);
			imageCount = capabilities.minImageCount;
		}

		// prevent exceeding the max image count
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			LOG_WARN("Querying more images than supported. imageCount set to maxImageCount.");
			imageCount = capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		// amount of layers each image consist of
		// it's 1 unless we are developing some 3D stereoscopic app
		createInfo.imageArrayLayers = 1;
		// if we want to render to a separate image first to perform post-processing
		// we should change this image usage
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | 
								VK_IMAGE_USAGE_TRANSFER_DST_BIT;// TODO: remove transfer dst if not drawing to swapchain image

		// don't support different graphics and present family
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// here we could specify a transformation to be applied on the images of the swap chain
		createInfo.preTransform = capabilities.currentTransform;

		// here we could blend the images with other windows in the window system
		// to ignore this blending we set OPAQUE
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		// here we clip pixels behind our window
		createInfo.clipped = true;

		// here we specify a handle to when the swapchain become invalid
		// possible causes are changing settings or resizing window
		createInfo.oldSwapchain = VK_NULL_HANDLE; // TODO: Pass current swapchain

		auto res = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
		DEBUG_VK(res, "Failed to create swap chain!");
		
		// DEBUG_TRACE("Creating swapchain with {} images.", imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImageResources.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImageResources.data());
		// DEBUG_TRACE("Created swapchain with {} images.", swapChainImageResources.size());
	}

	// create image views
	swapChainViews.resize(swapChainImageResources.size());
	swapChainImages.resize(swapChainImageResources.size());
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = swapChainImageResources[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = colorFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		auto res = vkCreateImageView(device, &viewInfo, _ctx.allocator, &swapChainViews[i]);
		DEBUG_VK(res, "Failed to create SwapChain image view!");

		swapChainImages[i].resource = std::make_shared<ImageResource>();
		swapChainImages[i].resource->fromSwapchain = true;
		swapChainImages[i].resource->image = swapChainImageResources[i];
		swapChainImages[i].resource->view = swapChainViews[i];
		swapChainImages[i].layout = ImageLayout::Undefined;
		swapChainImages[i].width = width;
		swapChainImages[i].height = height;
		swapChainImages[i].aspect = Aspect::Color;
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		std::string strName = ("SwapChain Image " + std::to_string(i));
		VkDebugUtilsObjectNameInfoEXT name = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE,
			.objectHandle = (uint64_t)(VkImage)swapChainImageResources[i],
			.pObjectName = strName.c_str(),
		};
		_ctx.vkSetDebugUtilsObjectNameEXT(device, &name);
		swapChainImages[i].resource->name = strName;
		strName = ("SwapChain View " + std::to_string(i));
		name = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE_VIEW,
			.objectHandle = (uint64_t)(VkImageView)swapChainViews[i],
			.pObjectName = strName.c_str(),
		};
		_ctx.vkSetDebugUtilsObjectNameEXT(device, &name);
	}

	if (!is_recreation)
	{
		// synchronization objects
		imageAvailableSemaphores.resize(framesInFlight);
		renderFinishedSemaphores.resize(framesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (size_t i = 0; i < framesInFlight; i++) {
			auto res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
			DEBUG_VK(res, "Failed to create semaphore!");
			res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
			DEBUG_VK(res, "Failed to create semaphore!");
		}

		// command buffers
		commands.resize(framesInFlight);
		for (auto& cmd: commands) {
			cmd.resource->queue = _ctx.queues[Queue::Graphics]; // TODO: make configurable
		}
		_ctx.CreateCommandBuffers(commands);
	}

	// LOG_INFO("Created Swapchain {}", _ctx.swapChains.size());
	currentFrame = 0;
	currentImageIndex = 0;
	dirty = false;
}

void SwapChain::Destroy(bool is_recreation) {
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		vkDestroyImageView(device, swapChainViews[i], _ctx.allocator);
	}
	
	if (!is_recreation) {
		for (size_t i = 0; i < framesInFlight; i++) {
			vkDestroySemaphore(device, imageAvailableSemaphores[i], _ctx.allocator);
			vkDestroySemaphore(device, renderFinishedSemaphores[i], _ctx.allocator);
		}
		imageAvailableSemaphores.clear();
		renderFinishedSemaphores.clear();
		availablePresentModes.clear();
		availableSurfaceFormats.clear();
	}

	vkDestroySwapchainKHR(device, swapChain, _ctx.allocator);
	if (!is_recreation) {
		vkDestroySurfaceKHR(_ctx.instance, surface, _ctx.allocator);
		_ctx.DestroyCommandBuffers(commands);
	}

	swapChainViews.clear();
	swapChainImages.clear();
	swapChain = VK_NULL_HANDLE;
}


bool AcquireImage(GLFWwindow* window) {
	auto& swapChain = _ctx.swapChains[window];
	auto res = vkAcquireNextImageKHR(_ctx.device, swapChain.swapChain, UINT64_MAX, swapChain.GetImageAvailableSemaphore(), VK_NULL_HANDLE, &swapChain.currentImageIndex);

	if (res == VK_ERROR_OUT_OF_DATE_KHR) {
		LOG_WARN("AcquireImage: Out of date");
		swapChain.dirty = true;
	} else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
		DEBUG_VK(res, "Failed to acquire swap chain image!");
	}

	if (res == VK_SUCCESS) {
		return true;
	} else {
		return false;
	}
}

bool GetSwapChainDirty(GLFWwindow* window) {
	if  (_ctx.swapChains.find(window) == _ctx.swapChains.end()) {
		// DEBUG_TRACE("GetSwapChainDirty s={}: Window not found", _ctx.swapChains.size());
		return true;
	};
	auto& swapChain = _ctx.swapChains[window];
	// DEBUG_TRACE("GetSwapChainDirty: s={} {} {}", _ctx.swapChains.size(), (void*)window, swapChain.dirty);	
	return swapChain.dirty;
}

// EndCommandBuffer + vkQueuePresentKHR
void SubmitAndPresent(GLFWwindow* window) {
	auto& swapChain = _ctx.swapChains[window];

	SubmitInfo submitInfo{};
	submitInfo.waitSemaphore   = (Semaphore*)swapChain.GetImageAvailableSemaphore();
	submitInfo.signalSemaphore = (Semaphore*)swapChain.GetRenderFinishedSemaphore();
	
	auto cmd = swapChain.GetCommandBuffer();
	cmd.EndCommandBuffer();
	cmd.QueueSubmit(submitInfo);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1; // TODO: pass with info
	presentInfo.pWaitSemaphores = (VkSemaphore*)&submitInfo.signalSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain.swapChain;
	presentInfo.pImageIndices = &swapChain.currentImageIndex;
	presentInfo.pResults = nullptr;

	auto res = vkQueuePresentKHR(swapChain.GetCommandBuffer().resource->queue->queue, &presentInfo); // TODO: use present queue

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
		if (res == VK_ERROR_OUT_OF_DATE_KHR) { LOG_WARN("vkQueuePresentKHR: Out of date") }
		if (res == VK_SUBOPTIMAL_KHR) { LOG_WARN("vkQueuePresentKHR: Suboptimal") }

		swapChain.dirty = true;
		return;
	}
	else if (res != VK_SUCCESS) {
		DEBUG_VK(res, "Failed to present swap chain image!");
	}

	swapChain.currentFrame = (swapChain.currentFrame + 1) % swapChain.framesInFlight;
}


const u32 MAX_STORAGE = 64;
const u32 MAX_SAMPLEDIMAGES = 64;
// const u32 MAX_ACCELERATIONSTRUCTURE = 64;
const u32 MAX_STORAGE_IMAGES = 8192;


void Context::createDescriptorPool(){
	// type                                     descriptorCount
	std::vector<VkDescriptorPoolSize> poolSizes = { 
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,        MAX_SAMPLEDIMAGES},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,                MAX_STORAGE},
		// {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, MAX_ACCELERATIONSTRUCTURE},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,                 MAX_STORAGE_IMAGES},
	};

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType	       = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	// descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;//////
	descriptorPoolCreateInfo.maxSets	   = 1; 
	descriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
	descriptorPoolCreateInfo.pPoolSizes    = poolSizes.data();
	VkResult result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, NULL, &descriptorPool);
	DEBUG_VK(result, "Failed to create descriptor pool!");
	ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool!");
}

void Context::createDescriptorSetLayout(){
	const int bindingCount = 3;
	std::vector<VkDescriptorSetLayoutBinding> bindings(bindingCount);
	std::vector<VkDescriptorBindingFlags> bindingFlags(bindingCount);

	VkDescriptorSetLayoutBinding texture{};
	bindings[0].binding = BINDING_TEXTURE;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[0].descriptorCount = MAX_SAMPLEDIMAGES;
	bindings[0].stageFlags = VK_SHADER_STAGE_ALL;
	// bindings.push_back(texture);
	// bindingFlags.push_back({ VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT });

	bindings[1].binding = BINDING_BUFFER;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings[1].descriptorCount = MAX_STORAGE;
	bindings[1].stageFlags = VK_SHADER_STAGE_ALL;
	bindingFlags[1] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;

	bindings[2].binding = BINDING_STORAGE_IMAGE;
	bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	bindings[2].descriptorCount = MAX_STORAGE_IMAGES;
	bindings[2].stageFlags = VK_SHADER_STAGE_ALL;


	// VkDescriptorSetLayoutBindingFlagsCreateInfo setLayoutBindingFlags{};
	// setLayoutBindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	// setLayoutBindingFlags.bindingCount = 2;
	// setLayoutBindingFlags.pBindingFlags = bindingFlags;

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType	= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = bindingCount; 
	descriptorSetLayoutCreateInfo.pBindings	= &bindings[0];
	// descriptorSetLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	// descriptorSetLayoutCreateInfo.pNext = &setLayoutBindingFlags;

	VkResult result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, NULL, &descriptorSetLayout);
	DEBUG_VK(result, "Failed to allocate descriptor set!");
	ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor set!");
}

void Context::createDescriptorSet(){
	// Descriptor Set
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
	DEBUG_VK(result, "Failed to allocate descriptor set!");
	ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor set!");
}

void Context::createDescriptorResources(){
	createDescriptorSetLayout();
	createDescriptorPool();
	createDescriptorSet();
}

// vkDestroyDescriptorPool + vkDestroyDescriptorSetLayout
void Context::destroyDescriptorResources(){
	vkDestroyDescriptorPool(device, descriptorPool, allocator);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, allocator);
	descriptorSet = VK_NULL_HANDLE;
	descriptorPool = VK_NULL_HANDLE;
	descriptorSetLayout = VK_NULL_HANDLE;
}

void Context::createBindlessResources(){
	// create bindless resources
	{

		// TODO: val = min(MAX_, available)
		const u32 MAX_STORAGE = 8192;
		const u32 MAX_SAMPLEDIMAGES = 8192;
		// const u32 MAX_ACCELERATIONSTRUCTURE = 64;
		const u32 MAX_STORAGE_IMAGES = 8192;

		// create descriptor set pool for bindless resources
		std::vector<VkDescriptorPoolSize> bindlessPoolSizes = { 
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_SAMPLEDIMAGES},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAX_STORAGE},
			// {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, MAX_ACCELERATIONSTRUCTURE},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, MAX_STORAGE_IMAGES},
		};

		// Fill with sequential numbers
		availableBufferRID.resize(MAX_STORAGE);
		std::iota(availableBufferRID.rbegin(), availableBufferRID.rend(), 0);
		availableImageRID.resize(MAX_SAMPLEDIMAGES);
		std::iota(availableImageRID.rbegin(), availableImageRID.rend(), 0);
		// availableTLASRID.resize(MAX_ACCELERATIONSTRUCTURE);
		// std::iota(availableTLASRID.begin(), availableTLASRID.end(), 0);

		VkDescriptorPoolCreateInfo bindlessPoolInfo{};
		bindlessPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		bindlessPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
		bindlessPoolInfo.maxSets = 1;
		bindlessPoolInfo.poolSizeCount = bindlessPoolSizes.size();
		bindlessPoolInfo.pPoolSizes = bindlessPoolSizes.data();

		VkResult result = vkCreateDescriptorPool(device, &bindlessPoolInfo, allocator, &bindlessDescriptorPool);
		DEBUG_VK(result, "Failed to create bindless descriptor pool!");
		ASSERT(result == VK_SUCCESS, "Failed to create bindless descriptor pool!");

		// create Descriptor Set Layout for bindless resources
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		std::vector<VkDescriptorBindingFlags> bindingFlags;

		VkDescriptorSetLayoutBinding texturesBinding{};
		texturesBinding.binding = BINDING_TEXTURE;
		texturesBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texturesBinding.descriptorCount = MAX_SAMPLEDIMAGES;
		texturesBinding.stageFlags = VK_SHADER_STAGE_ALL;
		bindings.push_back(texturesBinding);
		bindingFlags.push_back({ VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT });

		VkDescriptorSetLayoutBinding storageBuffersBinding{};
		storageBuffersBinding.binding = BINDING_BUFFER;
		storageBuffersBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		storageBuffersBinding.descriptorCount = MAX_STORAGE;
		storageBuffersBinding.stageFlags = VK_SHADER_STAGE_ALL;
		bindings.push_back(storageBuffersBinding);
		bindingFlags.push_back({ VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT });

		// VkDescriptorSetLayoutBinding accelerationStructureBinding{};
		// accelerationStructureBinding.binding = BINDING_TLAS;
		// accelerationStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		// accelerationStructureBinding.descriptorCount = MAX_ACCELERATIONSTRUCTURE;
		// accelerationStructureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		// bindings.push_back(accelerationStructureBinding);
		// bindingFlags.push_back({ VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT });

		VkDescriptorSetLayoutBinding imageStorageBinding{};
		imageStorageBinding.binding = BINDING_STORAGE_IMAGE;
		imageStorageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		imageStorageBinding.descriptorCount = MAX_STORAGE_IMAGES;
		imageStorageBinding.stageFlags = VK_SHADER_STAGE_ALL;
		bindings.push_back(imageStorageBinding);
		bindingFlags.push_back({ VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT });
		// TODO: try VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT for last binding ONLY

		VkDescriptorSetLayoutBindingFlagsCreateInfo setLayoutBindingFlags{};
		setLayoutBindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		setLayoutBindingFlags.bindingCount = bindingFlags.size();
		setLayoutBindingFlags.pBindingFlags = bindingFlags.data();

		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.bindingCount = bindings.size();
		descriptorLayoutInfo.pBindings = bindings.data();
		descriptorLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
		descriptorLayoutInfo.pNext = &setLayoutBindingFlags;

		result = vkCreateDescriptorSetLayout(device, &descriptorLayoutInfo, allocator, &bindlessDescriptorLayout);
		DEBUG_VK(result, "Failed to create bindless descriptor set layout!");
		
		// create Descriptor Set for bindless resources
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = bindlessDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &bindlessDescriptorLayout;

		result = vkAllocateDescriptorSets(device, &allocInfo, &bindlessDescriptorSet);
		DEBUG_VK(result, "Failed to allocate bindless descriptor set!");
		ASSERT(result == VK_SUCCESS, "Failed to allocate bindless descriptor set!");
	}

	// asScratchBuffer = vkw::CreateBuffer(initialScratchBufferSize, vkw::BufferUsage::Address | vkw::BufferUsage::Storage, vkw::Memory::GPU);
	// VkBufferDeviceAddressInfo scratchInfo{};
	// scratchInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	// scratchInfo.buffer = asScratchBuffer.resource->buffer;
	// asScratchAddress = vkGetBufferDeviceAddress(device, &scratchInfo);

	// dummyVertexBuffer = vkw::CreateBuffer(
	// 	6 * 3 * sizeof(float),
	// 	vkw::BufferUsage::Vertex | vkw::BufferUsage::AccelerationStructureInput,
	// 	vkw::Memory::GPU,
	// 	"VertexBuffer#Dummy"
	// );
}

void Context::destroyBindlessResources(){
	vkDestroyDescriptorPool(device, bindlessDescriptorPool, allocator);
	vkDestroyDescriptorSetLayout(device, bindlessDescriptorLayout, allocator);
	bindlessDescriptorSet = VK_NULL_HANDLE;
	bindlessDescriptorPool = VK_NULL_HANDLE;
	bindlessDescriptorLayout = VK_NULL_HANDLE;
}


void Context::CreateCommandBuffers(std::vector<Command>& commands) {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = 0; // do not use VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
	
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	static int buf_count = 0;
	for (auto& cmd: commands) {

		poolInfo.queueFamilyIndex = cmd.resource->queue->family;
		auto res = vkCreateCommandPool(device, &poolInfo, allocator, &cmd.resource->pool);
		DEBUG_VK(res, "Failed to create command pool!");

		allocInfo.commandPool = cmd.resource->pool;
		res = vkAllocateCommandBuffers(device, &allocInfo, &cmd.resource->buffer);
		DEBUG_VK(res, "Failed to allocate command buffer!");

		cmd.resource->staging = CreateBuffer(stagingBufferSize, BufferUsage::TransferSrc, Memory::CPU, "StagingBuffer[" + std::to_string(cmd.resource->queue->family) + "]" + std::to_string(buf_count++));
		cmd.resource->stagingCpu = (u8*)cmd.resource->staging.resource->allocation->GetMappedData();

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		vkCreateFence(device, &fenceInfo, allocator, &cmd.resource->fence);
		// DEBUG_TRACE("Created fence {}", (void*)cmd.resource->fence);

		// VkQueryPoolCreateInfo queryPoolInfo{};
		// queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
		// queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
		// queryPoolInfo.queryCount = timeStampPerPool;
		// res = vkCreateQueryPool(device, &queryPoolInfo, allocator, &cmd.resource->queryPool);
		// DEBUG_VK(res, "failed to create query pool");

		// cmd.resource->timeStamps.clear();
		// cmd.resource->timeStampNames.clear();

	}
}

void Context::DestroyCommandBuffers(std::vector<Command>& commands) {
	for (auto& cmd: commands) {
		vkDestroyCommandPool(device, cmd.resource->pool, allocator);
		cmd.resource->staging = {};
		cmd.resource->stagingCpu = nullptr;
		vkDestroyFence(device, cmd.resource->fence, allocator);
		// DEBUG_TRACE("Destroyed fence {}", (void*)cmd.resource->fence);
		// printf("Destroyed fence %p\n", cmd.resource->fence);
		// vkDestroyQueryPool(device, cmd.resource->queryPool, allocator);
	}
	commands.clear();
}

bool SwapChain::SupportFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) {
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(_ctx.physicalDevice, format, &props);

	if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
		return true;
	} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
		return true;
	}

	return false;
}

VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
	for (const auto& availableFormat : formats) {
		if (availableFormat.format == colorFormat
			&& availableFormat.colorSpace == colorSpace) {
			return availableFormat;
		}
	}
	LOG_WARN("Preferred surface format not available!");
	return formats[0];
}

VkPresentModeKHR SwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	for (const auto& mode : presentModes) {
		if (mode == presentMode) {
			return mode;
		}
	}
	LOG_WARN("Preferred present mode not available!");
	// FIFO is guaranteed to be available
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { width, height };

		actualExtent.width = std::max (
			capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width)
		);
		actualExtent.height = std::max (
			capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.height, actualExtent.height)
		);
		DEBUG_TRACE("ChooseExtent: {0}, {1}", actualExtent.width, actualExtent.height);
		return actualExtent;
	}
}



void Command::Copy(Buffer& dst, void* data, uint32_t size, uint32_t dstOfsset) {
	if (_ctx.stagingBufferSize - resource->stagingOffset < size) {
		LOG_ERROR("not enough size in staging buffer to copy");
		// todo: allocate additional buffer
		return;
	}
	// memcpy(resource->stagingCpu + resource->stagingOffset, data, size);
	vmaCopyMemoryToAllocation(_ctx.vmaAllocator, data, resource->staging.resource->allocation, resource->stagingOffset, size);
	Copy(dst, resource->staging, size, dstOfsset, resource->stagingOffset);
	DEBUG_TRACE("CmdCopy, size: {}, offset: {}", size, resource->stagingOffset);
	resource->stagingOffset += size;
}

void Command::Copy(Buffer& dst, Buffer& src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset) {
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(resource->buffer, src.resource->buffer, dst.resource->buffer, 1, &copyRegion);
}

void Command::Copy(Image& dst, void* data, uint32_t size) {
	if (_ctx.stagingBufferSize - resource->stagingOffset < size) {
		LOG_ERROR("not enough size in staging buffer to copy");
		// todo: allocate additional buffer
		return;
	}
	memcpy(resource->stagingCpu + resource->stagingOffset, data, size);
	Copy(dst, resource->staging, resource->stagingOffset);
	resource->stagingOffset += size;
}

void Command::Copy(Image& dst, Buffer& src, uint32_t srcOffset) {
	VkBufferImageCopy region{};
	region.bufferOffset = srcOffset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	ASSERT(!(dst.aspect & Aspect::Depth || dst.aspect & Aspect::Stencil), "CmdCopy don't support depth/stencil images");
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { dst.width, dst.height, 1 };
	vkCmdCopyBufferToImage(resource->buffer, src.resource->buffer, dst.resource->image, (VkImageLayout)dst.layout, 1, &region);
}

void Command::Copy(Buffer& dst, Image& src, uint32_t dstOffset) {
	VkBufferImageCopy region{};
	region.bufferOffset = dstOffset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	ASSERT(!(src.aspect & Aspect::Depth || src.aspect & Aspect::Stencil), "CmdCopy don't support depth/stencil images");
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {src.width, src.height, 1 };
	vkCmdCopyImageToBuffer(resource->buffer, src.resource->image, (VkImageLayout)src.layout, dst.resource->buffer, 1, &region);
}
// Vulkan 1.3 // 
void Command::Copy(Buffer &dst, Image &src, uint32_t dstOffset, ivec2 imageOffset, ivec2 imageExtent) {
// Command::void CmdCopy(Buffer &dst, Image &src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset){
	VkBufferImageCopy2 region{};
	region.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = {imageOffset[0], imageOffset[1], 0};
	region.imageExtent = {(uint32_t)imageExtent[0], (uint32_t)imageExtent[1], 1 };
	// region.imageExtent = {sizeX, sizeY, 1 };
	region.bufferOffset = dstOffset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	VkCopyImageToBufferInfo2 copyInfo{};
	copyInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
	copyInfo.srcImage = src.resource->image;
	copyInfo.srcImageLayout = (VkImageLayout)src.layout;
	copyInfo.dstBuffer = dst.resource->buffer;
	copyInfo.regionCount = 1;
	copyInfo.pRegions = &region;
	vkCmdCopyImageToBuffer2(resource->buffer, &copyInfo);
}

void Command::Barrier(Image& img, const ImageBarrier& barrier) {
	VkImageSubresourceRange range = {};
	range.aspectMask = (VkImageAspectFlags)img.aspect;
	range.baseMipLevel = 0;
	range.levelCount = VK_REMAINING_MIP_LEVELS;
	range.baseArrayLayer = 0;
	range.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VkImageMemoryBarrier2 barrier2 = {
		.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.pNext               = nullptr,
		.srcStageMask        = (VkPipelineStageFlags2) barrier.memoryBarrier.srcStageMask,
		.srcAccessMask       = (VkAccessFlags2)        barrier.memoryBarrier.srcAccessMask,
		.dstStageMask        = (VkPipelineStageFlags2) barrier.memoryBarrier.dstStageMask,
		.dstAccessMask       = (VkAccessFlags2)        barrier.memoryBarrier.dstAccessMask,
		.oldLayout           = (VkImageLayout)         (barrier.oldLayout == ImageLayout::MaxEnum ? img.layout : barrier.oldLayout),
		.newLayout           = (VkImageLayout)         barrier.newLayout,
		.srcQueueFamilyIndex = barrier.srcQueueFamilyIndex,
		.dstQueueFamilyIndex = barrier.dstQueueFamilyIndex,
		.image               = img.resource->image,
		.subresourceRange    = range
	};

	VkDependencyInfo dependency = {
		.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext                    = nullptr,
		.dependencyFlags          = 0,
		.memoryBarrierCount       = 0,
		.pMemoryBarriers          = nullptr,
		.bufferMemoryBarrierCount = 0,
		.pBufferMemoryBarriers    = nullptr,
		.imageMemoryBarrierCount  = 1,
		.pImageMemoryBarriers     = &barrier2
	};

	vkCmdPipelineBarrier2(resource->buffer, &dependency);
	img.layout = barrier.newLayout;
}

void Command::Barrier(Buffer& buf, const BufferBarrier& barrier) {

	VkBufferMemoryBarrier2 barrier2 {
		.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.pNext               = nullptr,
		.srcStageMask        = (VkPipelineStageFlags2) barrier.memoryBarrier.srcStageMask,
		.srcAccessMask       = (VkAccessFlags2)        barrier.memoryBarrier.srcAccessMask,
		.dstStageMask        = (VkPipelineStageFlags2) barrier.memoryBarrier.dstStageMask,
		.dstAccessMask       = (VkAccessFlags2)        barrier.memoryBarrier.dstAccessMask,
		.srcQueueFamilyIndex =                         barrier.srcQueueFamilyIndex,
		.dstQueueFamilyIndex =                         barrier.dstQueueFamilyIndex,
		.buffer              =                         buf.resource->buffer,
		.offset              = (VkDeviceSize)          barrier.offset,
		.size                = (VkDeviceSize)          barrier.size
	};

	VkDependencyInfo dependency = {
		.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext                    = nullptr,
		.dependencyFlags          = 0,
		.bufferMemoryBarrierCount = 1,
		.pBufferMemoryBarriers    = &barrier2,
	};
	vkCmdPipelineBarrier2(resource->buffer, &dependency);
}

void Command::Barrier(const MemoryBarrier& barrier) {
	VkMemoryBarrier2 barrier2 = {
		.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
		.pNext         = nullptr,
		.srcStageMask  = (VkPipelineStageFlags2) barrier.srcStageMask,
		.srcAccessMask = (VkAccessFlags2)        barrier.srcAccessMask,
		.dstStageMask  = (VkPipelineStageFlags2) barrier.dstStageMask,
		.dstAccessMask = (VkAccessFlags2)        barrier.dstAccessMask
	};
	VkDependencyInfo dependency = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.memoryBarrierCount = 1,
		.pMemoryBarriers = &barrier2,
	};
	vkCmdPipelineBarrier2(resource->buffer, &dependency);
}

void Command::ClearColorImage(Image& img, const float4& color) {
	VkClearColorValue clearColor{};
	clearColor.float32[0] = color.r;
	clearColor.float32[1] = color.g;
	clearColor.float32[2] = color.b;
	clearColor.float32[3] = color.a;
	VkImageSubresourceRange range = {};
	range.aspectMask = (VkImageAspectFlags)img.aspect;
	range.baseMipLevel = 0;
	range.levelCount = VK_REMAINING_MIP_LEVELS;
	range.baseArrayLayer = 0;
	range.layerCount = VK_REMAINING_ARRAY_LAYERS;

	vkCmdClearColorImage(resource->buffer, img.resource->image, (VkImageLayout)img.layout, &clearColor, 1, &range);
}

void Command::Blit(Image& dst, Image& src, ivec4 dstRegion, ivec4 srcRegion) {	
	if (dstRegion == ivec4{}) {dstRegion = {0, 0, (int)dst.width, (int)dst.height};}
	if (srcRegion == ivec4{}) {srcRegion = {0, 0, (int)src.width, (int)src.height};}

	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

	blitRegion.srcOffsets[0].x = srcRegion.x;
	blitRegion.srcOffsets[0].y = srcRegion.y;
	blitRegion.srcOffsets[0].z = 0;

	blitRegion.srcOffsets[1].x = srcRegion.z;
	blitRegion.srcOffsets[1].y = srcRegion.w;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[0].x = dstRegion.x;
	blitRegion.dstOffsets[0].y = dstRegion.y;
	blitRegion.dstOffsets[0].z = 0;

	blitRegion.dstOffsets[1].x = dstRegion.z;
	blitRegion.dstOffsets[1].y = dstRegion.w;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
	blitInfo.dstImage = dst.resource->image;
	blitInfo.dstImageLayout = (VkImageLayout)dst.layout;
	blitInfo.srcImage = src.resource->image;
	blitInfo.srcImageLayout = (VkImageLayout)src.layout;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2(resource->buffer, &blitInfo);
}

VkSampler Context::CreateSampler(VkDevice device, f32 maxLod) {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	// todo: create separate one for shadow maps
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;

	samplerInfo.anisotropyEnable = false; //?

	// what color to return when clamp is active in addressing mode
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	// if comparison is enabled, texels will be compared to a value an the result 
	// is used in filtering operations, can be used in PCF on shadow maps
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = maxLod;

	VkSampler sampler = VK_NULL_HANDLE;
	auto vkRes = vkCreateSampler(_ctx.device, &samplerInfo, nullptr, &sampler);
	DEBUG_VK(vkRes, "Failed to create texture sampler!");
	ASSERT(vkRes == VK_SUCCESS, "Failed to create texture sampler!");

	return sampler;
}


Image& GetCurrentSwapchainImage(GLFWwindow* window) {
	auto& swapChain = _ctx.swapChains[window];
	return swapChain.GetCurrentSwapChainImage();
}


}
static const char *VK_ERROR_STRING(VkResult result) {
	switch ((int)result)
	{
	case VK_SUCCESS: return "VK_SUCCESS";
	case VK_NOT_READY: return "VK_NOT_READY";
	case VK_TIMEOUT: return "VK_TIMEOUT";
	case VK_EVENT_SET: return "VK_EVENT_SET";
	case VK_EVENT_RESET: return "VK_EVENT_RESET";
	case VK_INCOMPLETE: return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
	case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
	case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
	case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
	case VK_PIPELINE_COMPILE_REQUIRED: return "VK_PIPELINE_COMPILE_REQUIRED";
	case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
	case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
	case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
	case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
	case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
	case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
	case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	case VK_ERROR_NOT_PERMITTED_KHR: return "VK_ERROR_NOT_PERMITTED_KHR";
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
	case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
	case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
	case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
	case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
	case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR: return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
	case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
	case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT: return "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT";
	// case VK_ERROR_OUT_OF_POOL_MEMORY_KHR: return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
	// case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
	// case VK_ERROR_FRAGMENTATION_EXT: return "VK_ERROR_FRAGMENTATION_EXT";
	// case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
	// case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
	// case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR";
	// case VK_PIPELINE_COMPILE_REQUIRED_EXT: return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
	// case VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT: return "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT";
	case VK_RESULT_MAX_ENUM: return "VK_RESULT_MAX_ENUM";
	default:
		if (result < 0)
			return "VK_ERROR_<Unknown>";
		return "VK_<Unknown>";
	}
}


namespace vkutil {

}