#include "Pch.hpp"

#include "VulkanBase.hpp"
#include "ShaderCommon.h"

#define SHADER_ALWAYS_COMPILE 0

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <GLFW/glfw3.h>

static const char *VK_ERROR_STRING(VkResult result);
namespace vkw {

void AcquireImage();

struct Context
{	
	void CmdCopy(Buffer& dst, void* data, uint32_t size, uint32_t dstOfsset);
	void CmdCopy(Buffer& dst, Buffer& src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset);
	void CmdCopy(Image& dst, void* data, uint32_t size);
	void CmdCopy(Image& dst, Buffer& src, uint32_t srcOffset);
	void CmdCopy(Buffer& dst, Image& src, uint32_t srcOffset);
	void CmdCopy(Buffer& dst, Image& src, uint32_t size, uint32_t dstOffset, ivec2 imageOffset, ivec2 imageExtent);
	void CmdBarrier(Image& img, Layout::ImageLayout newLayout, Layout::ImageLayout oldLayout);
	void CmdBarrier();
	void CmdClearColorImage(Image& image, float4& color);

	void CmdBlit(Image& dst, Image& src, uvec2 dstSize, uvec2 srcSize);

	void EndCommandBuffer(VkSubmitInfo submitInfo);

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
	struct CommandResources {
		u8* stagingCpu = nullptr;
		uint32_t stagingOffset = 0;
		Buffer staging;
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
		CommandResources* commands = nullptr; // Size = framesInFlight
	};

	
	bool requestSeparate[Queue::Count] = {false};
	std::unordered_map<uint32_t, InternalQueue> uniqueQueues;
	std::vector<CommandResources> commandResources; // Owns all command resources
	InternalQueue* queues[Queue::Count]; // Pointers to uniqueQueues
	Queue currentQueue = Queue::Count;

	std::shared_ptr<PipelineResource> currentPipeline;
	const uint32_t stagingBufferSize = 312 * 1024 * 1024;

	VkPhysicalDeviceMemoryProperties memoryProperties;


	// bindless resources
	// VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet bindlessDescriptorSet = VK_NULL_HANDLE;
	VkDescriptorPool bindlessDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout bindlessDescriptorLayout = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;

	struct SwapChainResource {
		VkSurfaceKHR surface = VK_NULL_HANDLE;	
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		std::vector<Image> swapChainImages;
		std::vector<VkImage> swapChainImageResources;
		std::vector<VkImageView> swapChainViews;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;

		
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<VkPresentModeKHR> availablePresentModes;
		std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;
		
		VkExtent2D extent;
		uint32_t swapChainCurrentFrame = 0;
		bool swapChainDirty = true;
		uint32_t currentImageIndex = 0;

		~SwapChainResource() {
			vkDestroySwapchainKHR(_ctx.device, swapChain, _ctx.allocator);
			vkDestroySurfaceKHR(_ctx.instance, surface, _ctx.allocator);
		}
	};
	// std::vector<SwapChainResource> swapChainResources;
	std::unordered_map<GLFWwindow*, SwapChainResource> swapChains;


	// VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	// std::vector<Image> swapChainImages;
	// std::vector<VkImage> swapChainImageResources;
	// std::vector<VkImageView> swapChainViews;
	// std::vector<VkSemaphore> imageAvailableSemaphores;
	// std::vector<VkSemaphore> renderFinishedSemaphores;

	uint32_t additionalImages = 0;
	uint32_t framesInFlight = 1;
	// VkFormat depthFormat;
	

	std::vector<int32_t> availableBufferRID;
	std::vector<int32_t> availableImageRID;
	std::vector<int32_t> availableTLASRID;
	VkSampler genericSampler;

	// // preferred, warn if not available
	VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	// VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
	VkColorSpaceKHR colorSpace  = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	VkSampleCountFlagBits numSamples  = VK_SAMPLE_COUNT_1_BIT;

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

	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSet();
	void createDescriptorResources(); // ALL:pool+layout+set
	void destroyDescriptorResources();

	void createBindlessResources();
	void destroyBindlessResources();

	void Context::CreateSurfaceFormats(SwapChainResource& swapChain);

	void CreateSwapChain(GLFWwindow* window, uint32_t width, uint32_t height);
	void DestroySwapChain();

	void createCommandBuffers();
	void DestroyCommandBuffers();


	uint32_t FindMemoryType(uint32_t type, VkMemoryPropertyFlags properties);
	bool SupportFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);


	inline Image& GetCurrentSwapChainImage() {
		return swapChainImages[currentImageIndex];
	}

	inline CommandResources& GetCurrentCommandResources() {
		return queues[currentQueue]->commands[swapChainCurrentFrame];
	}

	VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
	VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
	VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkSampler CreateSampler(f32 maxLod);

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

	if (_ctx.presentRequested) {
		_ctx.CreateSwapChain(window, width, height);
		// _ctx.CreateImGui(window);
	}
	
	_ctx.createCommandBuffers();
}

void Init() {
	InitImpl(nullptr, 0, 0);
}

void Init(GLFWwindow* window, uint32_t width, uint32_t height) {
	_ctx.presentRequested = true;
	_ctx.framesInFlight = 2;
	InitImpl(window, width, height);
}

void OnSurfaceUpdate(GLFWwindow* window, uint32_t width, uint32_t height) {
	_ctx.DestroySwapChain();
	_ctx.CreateSurfaceFormats(_ctx.swapChains[window].surface);
	_ctx.CreateSwapChain(window, width, height);
}


void Destroy() {
	// ImGui_ImplVulkan_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	for (auto& swapChain : _ctx.swapChains) {
		_ctx.DestroySwapChain(swapChain);
	}

	_ctx.destroyBindlessResources();
	// _ctx.destroyDescriptorResources();

	_ctx.DestroyCommandBuffers();
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
		.layout = Layout::Undefined,
		.aspect = aspect,
		.layers = desc.layers,
	};

	if (desc.usage & ImageUsage::Sampled || desc.usage & ImageUsage::Storage) {
		res->rid = _ctx.availableImageRID.back();
		_ctx.availableImageRID.pop_back();
	}

	if (desc.usage & ImageUsage::Sampled) {
		Layout::ImageLayout newLayout = Layout::ShaderRead;
		if (aspect == (Aspect::Depth | Aspect::Stencil)) {
			newLayout = Layout::DepthStencilRead;
		} else if (aspect == Aspect::Depth) {
			newLayout = Layout::DepthRead;
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


void CmdDispatch(const uvec3& groups) {
	auto& cmd = _ctx.GetCurrentCommandResources();
	vkCmdDispatch(cmd.buffer, groups.x, groups.y, groups.z);
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
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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


void CmdCopy(Buffer& dst, void* data, uint32_t size, uint32_t dstOfsset) {
	_ctx.CmdCopy(dst, data, size, dstOfsset);
}

void CmdCopy(Buffer& dst, Buffer& src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset) {
	_ctx.CmdCopy(dst, src, size, dstOffset, srcOffset);
}

void CmdCopy(Image& dst, void* data, uint32_t size) {
	_ctx.CmdCopy(dst, data, size); 
}

void CmdCopy(Image& dst, Buffer& src, uint32_t size, uint32_t srcOffset) {
	_ctx.CmdCopy(dst, src, srcOffset); 
}

void CmdCopy(Buffer& dst, Image& src, uint32_t size, uint32_t srcOffset) {
	_ctx.CmdCopy(dst, src, srcOffset);
}

void CmdCopy(Buffer &dst, Image &src, uint32_t size, uint32_t dstOffset, ivec2 imageOffset, ivec2 imageExtent){
	_ctx.CmdCopy(dst, src, size, dstOffset, imageOffset, imageExtent);
}

void CmdBarrier(Image& img, Layout::ImageLayout newLayout, Layout::ImageLayout oldLayout) {
	if (oldLayout == Layout::MaxEnum) {
		oldLayout = img.layout;
	}
	_ctx.CmdBarrier(img, newLayout, oldLayout);
}

void CmdBarrier() {
	_ctx.CmdBarrier();
}

void CmdBlit(Image& dst, Image& src, uvec2 dstSize, uvec2 srcSize) {
	if (dstSize.x == 0 && dstSize.y == 0) {dstSize.x = dst.width; dstSize.y = dst.height;}
	if (srcSize.x == 0 && srcSize.y == 0) {srcSize.x = src.width; srcSize.y = src.height;}
	_ctx.CmdBlit(dst, src, dstSize, srcSize);
}

void CmdClearColorImage(Image &image, float4 color)
{
	_ctx.CmdClearColorImage(image, color);
}

void CmdBeginRendering(const std::vector<Image>& colorAttachs, Image depthAttach, uint32_t layerCount) {
	auto& cmd = _ctx.GetCurrentCommandResources();

	ivec2 offset(0, 0);
	uvec2 extent(0, 0);
	if (colorAttachs.size() > 0) {
		extent.x = colorAttachs[0].width;
		extent.y = colorAttachs[0].height;
	} else if (depthAttach.resource) {
		extent.x = depthAttach.width;
		extent.y = depthAttach.height;
	}

	VkRenderingInfoKHR renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	renderingInfo.viewMask = 0;
	renderingInfo.layerCount = layerCount;
	renderingInfo.renderArea.extent = { uint32_t(extent.x), uint32_t(extent.y) };
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

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.x);
	viewport.height = static_cast<float>(extent.y);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor = {};
	scissor.offset = { offset.x, offset.y };
	scissor.extent.width = extent.x;
	scissor.extent.height = extent.y;
	vkCmdSetViewport(cmd.buffer, 0, 1, &viewport);
	vkCmdSetScissor(cmd.buffer, 0, 1, &scissor);

	vkCmdBeginRendering(cmd.buffer, &renderingInfo);
}

void CmdEndRendering() {
	auto& cmd = _ctx.GetCurrentCommandResources();
	vkCmdEndRendering(cmd.buffer);
}
// Acquire + CmdBarrier + CmdBeginRendering
void CmdBeginPresent() {
	vkw::AcquireImage();
	vkw::CmdBarrier(_ctx.GetCurrentSwapChainImage(), vkw::Layout::ColorAttachment);
	vkw::CmdBeginRendering({ _ctx.GetCurrentSwapChainImage() }, {});
}
// CmdEndRendering + CmdBarrier
void CmdEndPresent() {
	vkw::CmdEndRendering();
	vkw::CmdBarrier(_ctx.GetCurrentSwapChainImage(), vkw::Layout::Present);
}

void CmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
	auto& cmd = _ctx.GetCurrentCommandResources();
	DEBUG_TRACE("CmdDraw({},{},{},{})", vertexCount, instanceCount, firstVertex, firstInstance);
	vkCmdDraw(cmd.buffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CmdDrawMesh(Buffer& vertexBuffer, Buffer& indexBuffer, uint32_t indexCount) {
	auto& cmd = _ctx.GetCurrentCommandResources();
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd.buffer, 0, 1, &vertexBuffer.resource->buffer, offsets);
	vkCmdBindIndexBuffer(cmd.buffer, indexBuffer.resource->buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmd.buffer, indexCount, 1, 0, 0, 0);
}

void CmdBindVertexBuffer(Buffer& vertexBuffer) {
	auto& cmd = _ctx.GetCurrentCommandResources();
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd.buffer, 0, 1, &vertexBuffer.resource->buffer, offsets);
}

void CmdDrawLineStrip(const Buffer& pointsBuffer, uint32_t firstPoint, uint32_t pointCount, float thickness) {
	auto& cmd = _ctx.GetCurrentCommandResources();
	VkDeviceSize offsets[] = { 0 };
	vkCmdSetLineWidth(cmd.buffer, thickness);
	vkCmdBindVertexBuffers(cmd.buffer, 0, 1, &pointsBuffer.resource->buffer, offsets);
	vkCmdDraw(cmd.buffer, pointCount, 1, firstPoint, 0);
}

// void CmdDrawPassThrough() {
//     auto& cmd = _ctx.GetCurrentCommandResources();
//     VkDeviceSize offsets[] = { 0 };
//     vkCmdBindVertexBuffers(cmd.buffer, 0, 1, &_ctx.dummyVertexBuffer.resource->buffer, offsets);
//     vkCmdDraw(cmd.buffer, 6, 1, 0, 0);
// }

// void CmdDrawImGui(ImDrawData* data) {
//     ImGui_ImplVulkan_RenderDrawData(data, _ctx.GetCurrentCommandResources().buffer);
// }


// int CmdBeginTimeStamp(const std::string& name) {
//     DEBUG_ASSERT(_ctx.currentQueue != Queue::Transfer, "Time Stamp not supported in Transfer queue");
//     auto& cmd = _ctx.GetCurrentCommandResources();
//     int id = cmd.timeStamps.size();
//     if (id >= _ctx.timeStampPerPool - 1) {
//         LOG_WARN("Maximum number of time stamp per pool exceeded. Ignoring Time stamp {}", name.c_str());
//         return -1;
//     }
//     vkCmdWriteTimestamp(cmd.buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, cmd.queryPool, id);
//     cmd.timeStamps.push_back(0);
//     cmd.timeStamps.push_back(0);
//     cmd.timeStampNames.push_back(name);
//     return id;
// }

// void CmdEndTimeStamp(int timeStampIndex) {
//     if (timeStampIndex >= 0 && timeStampIndex < _ctx.timeStampPerPool - 1) {
//         auto& cmd = _ctx.GetCurrentCommandResources();
//         vkCmdWriteTimestamp(cmd.buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, cmd.queryPool, timeStampIndex + 1);
//     }
// }

void CmdBindPipeline(Pipeline& pipeline) {
	auto& cmd = _ctx.GetCurrentCommandResources();
	vkCmdBindPipeline(cmd.buffer, (VkPipelineBindPoint)pipeline.point, pipeline.resource->pipeline);
	// TODO: bind only if not compatible for used descriptor sets or push constant range
	// ref: https://registry.khronos.org/vulkan/specs/1.2-extensions/html/vkspec.html#descriptorsets-compatibility
	vkCmdBindDescriptorSets(cmd.buffer, (VkPipelineBindPoint)pipeline.point, pipeline.resource->layout, 0, 1, &_ctx.bindlessDescriptorSet, 0, nullptr);
	// vkCmdBindDescriptorSets(cmd.buffer, (VkPipelineBindPoint)pipeline.point, pipeline.resource->layout, 0, 1, &_ctx.descriptorSet, 0, nullptr);

	_ctx.currentPipeline = pipeline.resource;
}

void CmdPushConstants(void* data, uint32_t size) {
	auto& cmd = _ctx.GetCurrentCommandResources();
	vkCmdPushConstants(cmd.buffer, _ctx.currentPipeline->layout, VK_SHADER_STAGE_ALL, 0, size, data);
}

// void BeginImGui() {
//     ImGui_ImplVulkan_NewFrame();
//     ImGui_ImplGlfw_NewFrame();
// }

// vkWaitForFences + vkResetFences +
// vkResetCommandPool + vkBeginCommandBuffer
void BeginCommandBuffer(Queue queue) {
	ASSERT(_ctx.currentQueue == Queue::Count, "Already recording a command buffer");
	_ctx.currentQueue = queue;
	auto& cmd = _ctx.GetCurrentCommandResources();
	vkWaitForFences(_ctx.device, 1, &cmd.fence, VK_TRUE, UINT64_MAX);
	vkResetFences(_ctx.device, 1, &cmd.fence);

	// if (cmd.timeStamps.size() > 0) {
	//     vkGetQueryPoolResults(_ctx.device, cmd.queryPool, 0, cmd.timeStamps.size(), cmd.timeStamps.size() * sizeof(uint64_t), cmd.timeStamps.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);
	//     for (int i = 0; i < cmd.timeStampNames.size(); i++) {
	//         const uint64_t begin = cmd.timeStamps[2 * i];
	//         const uint64_t end = cmd.timeStamps[2 * i + 1];
	//         _ctx.timeStampTable[cmd.timeStampNames[i]] = float(end - begin) * _ctx.physicalProperties.limits.timestampPeriod / 1000000.0f;
	//     }
	//     cmd.timeStamps.clear();
	//     cmd.timeStampNames.clear();
	// }

	// ?VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
	vkResetCommandPool(_ctx.device, cmd.pool, 0); // TODO: check if this is needed
	cmd.stagingOffset = 0;
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmd.buffer, &beginInfo);

	// if (queue != Queue::Transfer) {
	//     vkCmdResetQueryPool(cmd.buffer, cmd.queryPool, 0, _ctx.timeStampPerPool);
	// }
}

void Context::EndCommandBuffer(VkSubmitInfo submitInfo) {
	auto& cmd = GetCurrentCommandResources();

	vkEndCommandBuffer(cmd.buffer);

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd.buffer;

	auto res = vkQueueSubmit(queues[currentQueue]->queue, 1, &submitInfo, cmd.fence);
	DEBUG_VK(res, "Failed to submit command buffer");
}

// vkEndCommandBuffer + vkQueueSubmit
void EndCommandBuffer() {
	_ctx.EndCommandBuffer({});
	_ctx.currentQueue = vkw::Queue::Count;
	_ctx.currentPipeline = {};
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
		DEBUG_TRACE("Created debug messenger., res = {}", (uint64_t)debugMessenger);
	}

	// Debug Report
	if (enableValidationLayers && enableDebugReport) {
		VkDebugReportCallbackCreateInfoEXT debugReportInfo;
		PopulateDebugReportCreateInfo(debugReportInfo);
		// Create the callback handle
		res = CreateDebugReportCallbackEXT(instance, &debugReportInfo, nullptr, &debugReport);
		// DEBUG_VK(res, "Failed to set up debug report callback!");
		DEBUG_TRACE("Created debug report callback., res = {}", (uint64_t)debugReport);
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
	if (_ctx.presentRequested){
		for (auto surface: surfaces){
			vkDestroySurfaceKHR(instance, surface, allocator);
		}
		surfaces.clear();
		DEBUG_TRACE("Destroyed surfaces.");
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
				return {i, 0}; // queue in family i with index 0
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

	// uint32_t maxQueuesInFamily = 0;
	// for (uint32_t i = 0; i < familyCount; i++) {
	// 	if (availableFamilies[i].queueCount > maxQueuesInFamily) maxQueuesInFamily = availableFamilies[i].queueCount;
	// }

	uint32_t maxQueuesInFamily = std::max_element(availableFamilies.begin(), availableFamilies.end(), [](const auto& a, const auto& b) {
		return a.queueCount < b.queueCount;
	}).base()->queueCount;
	
	auto queueKey = [&maxQueuesInFamily](uint32_t family, uint32_t index) -> uint32_t {
		return family*maxQueuesInFamily + index;
	};

	// requestSeparate[Queue::Transfer] = true;

	for (uint32_t i = 0; i < Queue::Count; i++) {
		uint32_t queueFamily, indexInFamily, key;
		bool is_separate;
		if (requestSeparate[i]) {
			std::tie(queueFamily, indexInFamily, is_separate) = find_separate((VkQueueFlagBits)i);
		} else {
			std::tie(queueFamily, indexInFamily) = find_first((VkQueueFlagBits)i);
		}
		key = queueKey(queueFamily, indexInFamily);
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

	// features2.pNext = &atomicFeatures;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	// createInfo.queueCreateInfoCount = 1;
	// createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	// createInfo.pEnabledFeatures; // !Should be NULL if pNext is used
	// createInfo.pNext = &features2; // feature chain
	createInfo.pNext = &sync2Features; // feature chain


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

	genericSampler = CreateSampler(1.0);
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


void Context::CreateSurfaceFormats(SwapChainResource& swapChain) {

	// get capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, swapChain.surface, &swapChain.surfaceCapabilities);

	// get surface formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, swapChain.surface, &formatCount, nullptr);
	if (formatCount != 0) {
		swapChain.availableSurfaceFormats.clear();
		swapChain.availableSurfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, swapChain.surface, &formatCount, swapChain.availableSurfaceFormats.data());
	}

	// get present modes
	uint32_t modeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, swapChain.surface, &modeCount, nullptr);
	if (modeCount != 0) {
		swapChain.availablePresentModes.clear();
		swapChain.availablePresentModes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, swapChain.surface, &modeCount, swapChain.availablePresentModes.data());
	}

	// set this device as not suitable if no surface formats or present modes available
	bool suitable = (modeCount > 0);
	suitable &= (formatCount > 0);

	if (!suitable) {
		LOG_ERROR("selected device invalidated after surface update.");
	}
}

void Context::CreateSwapChain(GLFWwindow* glfwWindow, uint32_t width, uint32_t height) {

	if (numSamples > maxSamples) {
		numSamples = maxSamples;
	}
	swapChains.try_emplace(glfwWindow);//.second;
	auto& swapChain = swapChains[glfwWindow];
	
	VkResult res = glfwCreateWindowSurface(_ctx.instance, glfwWindow, _ctx.allocator, &swapChain.surface);
	DEBUG_VK(res, "Failed to create window surface!");
	DEBUG_TRACE("Created surface.");

	_ctx.CreateSurfaceFormats(swapChain);

	// create swapchain
	{
		const auto& capabilities = swapChain.surfaceCapabilities;
		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapChain.availableSurfaceFormats);
		colorFormat = surfaceFormat.format;
		colorSpace = surfaceFormat.colorSpace;
		presentMode = ChoosePresentMode(swapChain.availablePresentModes);
		swapChain.extent = ChooseExtent(capabilities, width, height);

		// acquire additional images to prevent waiting for driver's internal operations
		uint32_t imageCount = framesInFlight + additionalImages;
		
		if (imageCount < capabilities.minImageCount) {
			LOG_WARN("Querying less images than the necessary!");
			imageCount = capabilities.minImageCount;
		}

		// prevent exceeding the max image count
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			LOG_WARN("Querying more images than supported. imageCount set to maxImageCount.");
			imageCount = capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = swapChain.surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = swapChain.extent;
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

		auto res = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain.swapChain);
		DEBUG_VK(res, "Failed to create swap chain!");

		vkGetSwapchainImagesKHR(device, swapChain.swapChain, &imageCount, nullptr);
		swapChain.swapChainImageResources.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain.swapChain, &imageCount, swapChain.swapChainImageResources.data());
	}

	// create image views
	swapChain.swapChainViews.resize(swapChain.swapChainImageResources.size());
	swapChain.swapChainImages.resize(swapChain.swapChainImageResources.size());
	for (size_t i = 0; i < swapChain.swapChainImages.size(); i++) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = swapChain.swapChainImageResources[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = colorFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		auto res = vkCreateImageView(device, &viewInfo, allocator, &swapChain.swapChainViews[i]);
		DEBUG_VK(res, "Failed to create swapChain.SwapChain image view!");

		swapChain.swapChainImages[i].resource = std::make_shared<ImageResource>();
		swapChain.swapChainImages[i].resource->fromSwapchain = true;
		swapChain.swapChainImages[i].resource->image = swapChain.swapChainImageResources[i];
		swapChain.swapChainImages[i].resource->view = swapChain.swapChainViews[i];
		swapChain.swapChainImages[i].layout = Layout::Undefined;
		swapChain.swapChainImages[i].width = width;
		swapChain.swapChainImages[i].height = height;
		swapChain.swapChainImages[i].aspect = Aspect::Color;
	}

	for (size_t i = 0; i < swapChain.swapChainImages.size(); i++) {
		std::string strName = ("SwapChain Image " + std::to_string(i));
		VkDebugUtilsObjectNameInfoEXT name = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE,
			.objectHandle = (uint64_t)(VkImage)swapChain.swapChainImageResources[i],
			.pObjectName = strName.c_str(),
		};
		_ctx.vkSetDebugUtilsObjectNameEXT(_ctx.device, &name);
		strName = ("SwapChain View " + std::to_string(i));
		name = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE_VIEW,
			.objectHandle = (uint64_t)(VkImageView)swapChain.swapChainViews[i],
			.pObjectName = strName.c_str(),
		};
		_ctx.vkSetDebugUtilsObjectNameEXT(_ctx.device, &name);
	}

	// synchronization objects
	{
		swapChain.imageAvailableSemaphores.resize(framesInFlight);
		swapChain.renderFinishedSemaphores.resize(framesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (size_t i = 0; i < framesInFlight; i++) {
			auto res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &swapChain.imageAvailableSemaphores[i]);
			DEBUG_VK(res, "Failed to create semaphore!");
			res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &swapChain.renderFinishedSemaphores[i]);
			DEBUG_VK(res, "Failed to create semaphore!");
		}
	}

	LOG_INFO("Created Swapchain");
	swapChain.swapChainCurrentFrame = 0;
	swapChain.currentImageIndex = 0;
	swapChain.swapChainDirty = false;
}

void Context::DestroySwapChain() {
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		vkDestroyImageView(device, swapChainViews[i], allocator);
	}

	for (size_t i = 0; i < framesInFlight; i++) {
		vkDestroySemaphore(device, imageAvailableSemaphores[i], allocator);
		vkDestroySemaphore(device, renderFinishedSemaphores[i], allocator);
	}

	vkDestroySwapchainKHR(device, swapChain, allocator);

	imageAvailableSemaphores.clear();
	renderFinishedSemaphores.clear();
	swapChainViews.clear();
	swapChainImages.clear();
	swapChain = VK_NULL_HANDLE;
}

void AcquireImage() {
	auto res = vkAcquireNextImageKHR(_ctx.device, _ctx.swapChain, UINT64_MAX, _ctx.imageAvailableSemaphores[_ctx.swapChainCurrentFrame], VK_NULL_HANDLE, &_ctx.currentImageIndex);

	if (res == VK_ERROR_OUT_OF_DATE_KHR) {
		_ctx.swapChainDirty = true;
	} else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
		DEBUG_VK(res, "Failed to acquire swap chain image!");
	}
}

bool GetSwapChainDirty() {
	return _ctx.swapChainDirty;
}
// EndCommandBuffer + vkQueuePresentKHR
void SubmitAndPresent() {
	auto& cmd = _ctx.GetCurrentCommandResources();

	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &_ctx.imageAvailableSemaphores[_ctx.swapChainCurrentFrame];
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(cmd.buffer);
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &_ctx.renderFinishedSemaphores[_ctx.swapChainCurrentFrame];

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = submitInfo.pSignalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &_ctx.swapChain;
	presentInfo.pImageIndices = &_ctx.currentImageIndex;
	presentInfo.pResults = nullptr;

	_ctx.EndCommandBuffer(submitInfo);

	auto res = vkQueuePresentKHR(_ctx.queues[_ctx.currentQueue]->queue, &presentInfo); // TODO: use present queue
	_ctx.currentQueue = Queue::Count;

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
		_ctx.swapChainDirty = true;
		return;
	}
	else if (res != VK_SUCCESS) {
		DEBUG_VK(res, "Failed to present swap chain image!");
	}

	_ctx.swapChainCurrentFrame = (_ctx.swapChainCurrentFrame + 1) % _ctx.framesInFlight;
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


void Context::createCommandBuffers(){
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = 0; // ?VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	commandResources.resize(uniqueQueues.size() * framesInFlight);

	uint32_t command_counter = 0;
	for (auto& [key, q]: uniqueQueues) {
		q.commands = commandResources.data() + command_counter * framesInFlight;
		command_counter++;
	}

	// DEBUG_TRACE("Queue[{}]: [{}][{}], commands.size = {}", q, queue.family, queue.indexInFamily, queue.commands.size());
		
	for (auto& [key, queue]: uniqueQueues) {
		// DEBUG_TRACE("Queue[{}]: [{}][{}]", key, queue.family, queue.indexInFamily);
		poolInfo.queueFamilyIndex = queue.family;
		for (int i = 0; i < framesInFlight; i++) {
			// DEBUG_TRACE("  Frame[{}]: [{}][{}]", i, queue.family, queue.indexInFamily);
			auto res = vkCreateCommandPool(device, &poolInfo, allocator, &queue.commands[i].pool);
			DEBUG_VK(res, "Failed to create command pool!");

			allocInfo.commandPool = queue.commands[i].pool;
			res = vkAllocateCommandBuffers(device, &allocInfo, &queue.commands[i].buffer);
			DEBUG_VK(res, "Failed to allocate command buffer!");

			queue.commands[i].staging = CreateBuffer(stagingBufferSize, BufferUsage::TransferSrc, Memory::CPU, "StagingBuffer[" + std::to_string(queue.family) + "][" + std::to_string(queue.indexInFamily) + "_" + std::to_string(i));
			queue.commands[i].stagingCpu = (u8*)queue.commands[i].staging.resource->allocation->GetMappedData();

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			vkCreateFence(device, &fenceInfo, allocator, &queue.commands[i].fence);

			// VkQueryPoolCreateInfo queryPoolInfo{};
			// queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
			// queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
			// queryPoolInfo.queryCount = timeStampPerPool;
			// res = vkCreateQueryPool(device, &queryPoolInfo, allocator, &queue.commands[i].queryPool);
			// DEBUG_VK(res, "failed to create query pool");

			// queue.commands[i].timeStamps.clear();
			// queue.commands[i].timeStampNames.clear();
		}
		// DEBUG_TRACE("Queue[{}]: [{}][{}] created", key, queue.family, queue.indexInFamily);
	}
	// DEBUG_TRACE("Created {} command buffers", commandResources.size());
}

void Context::DestroyCommandBuffers() {
	// for (int q = 0; q < Queue::Count; q++) {
	for (int q = 0; q < uniqueQueues.size(); q++) {
		for (int i = 0; i < framesInFlight; i++) {
			// vkFreeCommandBuffers(device, queues[q]->commands[i].pool, 1, &queues[q]->commands[i].buffer); // No OP
			vkDestroyCommandPool(device, queues[q]->commands[i].pool, allocator);
			queues[q]->commands[i].staging = {};
			queues[q]->commands[i].stagingCpu = nullptr;
			vkDestroyFence(device, queues[q]->commands[i].fence, allocator);
			// vkDestroyQueryPool(device, queues[q]->commands[i].queryPool, allocator);
		}
	}
}

uint32_t Context::FindMemoryType(uint32_t type, VkMemoryPropertyFlags properties) {
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if (type & (1 << i)) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
	}

	ASSERT(false, "failed to find suitable memory type");
}


bool Context::SupportFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) {
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

	if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
		return true;
	} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
		return true;
	}

	return false;
}

VkSurfaceFormatKHR Context::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
	for (const auto& availableFormat : formats) {
		if (availableFormat.format == colorFormat
			&& availableFormat.colorSpace == colorSpace) {
			return availableFormat;
		}
	}
	LOG_WARN("Preferred surface format not available!");
	return formats[0];
}

VkPresentModeKHR Context::ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	for (const auto& mode : presentModes) {
		if (mode == presentMode) {
			return mode;
		}
	}
	LOG_WARN("Preferred present mode not available!");
	// FIFO is guaranteed to be available
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Context::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
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

		return actualExtent;
	}
}



void Context::CmdCopy(Buffer& dst, void* data, uint32_t size, uint32_t dstOfsset) {
	CommandResources& cmd = GetCurrentCommandResources();
	if (stagingBufferSize - cmd.stagingOffset < size) {
		LOG_ERROR("not enough size in staging buffer to copy");
		// todo: allocate additional buffer
		return;
	}
	memcpy(cmd.stagingCpu + cmd.stagingOffset, data, size);
	CmdCopy(dst, cmd.staging, size, dstOfsset, cmd.stagingOffset);
	cmd.stagingOffset += size;
}

void Context::CmdCopy(Buffer& dst, Buffer& src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset) {
	CommandResources& cmd = GetCurrentCommandResources();
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(cmd.buffer, src.resource->buffer, dst.resource->buffer, 1, &copyRegion);
}

void Context::CmdCopy(Image& dst, void* data, uint32_t size) {
	CommandResources& cmd = GetCurrentCommandResources();
	if (stagingBufferSize - cmd.stagingOffset < size) {
		LOG_ERROR("not enough size in staging buffer to copy");
		// todo: allocate additional buffer
		return;
	}
	memcpy(cmd.stagingCpu + cmd.stagingOffset, data, size);
	CmdCopy(dst, cmd.staging, cmd.stagingOffset);
	cmd.stagingOffset += size;
}

void Context::CmdCopy(Image& dst, Buffer& src, uint32_t srcOffset) {
	CommandResources& cmd = GetCurrentCommandResources();
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
	vkCmdCopyBufferToImage(cmd.buffer, src.resource->buffer, dst.resource->image, (VkImageLayout)dst.layout, 1, &region);
}

void Context::CmdCopy(Buffer& dst, Image& src, uint32_t dstOffset) {
	CommandResources& cmd = GetCurrentCommandResources();
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
	vkCmdCopyImageToBuffer(cmd.buffer, src.resource->image, (VkImageLayout)src.layout, dst.resource->buffer, 1, &region);
}
// Vulkan 1.3 // 
void Context::CmdCopy(Buffer &dst, Image &src, uint32_t size, uint32_t dstOffset, ivec2 imageOffset, ivec2 imageExtent) {
// void Context::CmdCopy(Buffer &dst, Image &src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset){
	CommandResources& cmd = GetCurrentCommandResources();
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
	vkCmdCopyImageToBuffer2(cmd.buffer, &copyInfo);
}

void Context::CmdBarrier(Image& img, Layout::ImageLayout newLayout, Layout::ImageLayout oldLayout) {
	CommandResources& cmd = GetCurrentCommandResources();
	VkImageSubresourceRange range = {};
	range.aspectMask = (VkImageAspectFlags)img.aspect;
	range.baseMipLevel = 0;
	range.levelCount = VK_REMAINING_MIP_LEVELS;
	range.baseArrayLayer = 0;
	range.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VkAccessFlags srcAccess = VK_ACCESS_SHADER_WRITE_BIT;
	VkAccessFlags dstAccess = VK_ACCESS_SHADER_READ_BIT;
	VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // Bottom
	VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // Top

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcAccessMask = srcAccess;
	barrier.dstAccessMask = dstAccess;
	barrier.oldLayout = (VkImageLayout)oldLayout;
	barrier.newLayout = (VkImageLayout)newLayout;
	barrier.image = img.resource->image;
	barrier.subresourceRange = range;
	vkCmdPipelineBarrier(cmd.buffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	img.layout = newLayout;
}

void Context::CmdBarrier() {
	VkMemoryBarrier2 barrier = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
	};
	VkDependencyInfo dependency = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.memoryBarrierCount = 1,
		.pMemoryBarriers = &barrier,
	};
	vkCmdPipelineBarrier2(GetCurrentCommandResources().buffer, &dependency);
}

void Context::CmdClearColorImage(Image& img, float4& color) {
	CommandResources& cmd = GetCurrentCommandResources();
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

	vkCmdClearColorImage(cmd.buffer, img.resource->image, (VkImageLayout)img.layout, &clearColor, 1, &range);
}

void Context::CmdBlit(Image& dst, Image& src, uvec2 dstSize, uvec2 srcSize) {	
	auto& cmd = GetCurrentCommandResources();

	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

	blitRegion.srcOffsets[1].x = srcSize.x;
	blitRegion.srcOffsets[1].y = srcSize.y;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[1].x = dstSize.x;
	blitRegion.dstOffsets[1].y = dstSize.y;
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
	// blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; // TODO: change
	blitInfo.dstImageLayout = (VkImageLayout)dst.layout;
	blitInfo.srcImage = src.resource->image;
	// blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; // TODO: change
	blitInfo.srcImageLayout = (VkImageLayout)src.layout;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2(cmd.buffer, &blitInfo);
}

VkSampler Context::CreateSampler(f32 maxLod) {
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
	auto vkRes = vkCreateSampler(device, &samplerInfo, nullptr, &sampler);
	DEBUG_VK(vkRes, "Failed to create texture sampler!");
	ASSERT(vkRes == VK_SUCCESS, "Failed to create texture sampler!");

	return sampler;
}


Image& GetCurrentSwapchainImage() {
	return _ctx.GetCurrentSwapChainImage();
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