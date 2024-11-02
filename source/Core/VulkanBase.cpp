#include "Log.hpp"
#include "Base.hpp"
#include "Util.hpp"
#include "VulkanBase.hpp"
#include "ShaderCommon.h"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <numeric>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vulkan/vulkan_core.h>
#include <fstream>
#include <span>

#define SHADER_ALWAYS_COMPILE 0

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#define IMGUI_VULKAN_DEBUG_REPORT
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

static const char *VK_ERROR_STRING(VkResult result);
namespace vkw {
struct Instance;
struct PhysicalDevice;
struct DeviceResource;
struct SwapChain;
struct Context
{	
	// void EndCommandBuffer(VkSubmitInfo submitInfo);
	// Command::void EndCommandBuffer(VkSubmitInfo submitInfo);
	VkAllocationCallbacks* allocator = VK_NULL_HANDLE;


	void LoadShaders(Pipeline& pipeline);
	std::vector<char> CompileShader(const std::filesystem::path& path, const char* entryPoint);

	void CreatePipelineLibrary(const PipelineDesc& desc, Pipeline& pipeline);

	bool presentRequested = false;
	bool enableValidationLayers = true;
	bool enableDebugReport = false;
	bool linkTimeOptimization = true; // Pipeline library link

	std::vector<const char*> requiredInstanceExtensions {
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
	};

	std::vector<const char*> preferredExtensions = {
		VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
		VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
	};

	std::unordered_map<UID, PhysicalDevice> physicalDevices;
	std::unordered_map<UID, std::shared_ptr<DeviceResource>> devices;

	std::shared_ptr<Instance> instance;
	// std::shared_ptr<PhysicalDevice> activePhysicalDevice;
	// std::shared_ptr<DeviceResource> activeDevice;

	const uint32_t timeStampPerPool = 64;

	// bool requestSeparate[QueueFlagBits::Count] = {false};
	
	// const uint32_t initialScratchBufferSize = 64*1024*1024;
	// Buffer asScratchBuffer;
	// VkDeviceAddress asScratchAddress;
	// Buffer dummyVertexBuffer;


	// std::map<std::string, float> timeStampTable;

	void GetPhysicalDevices();
	std::vector<UID> SelectPhysicalDevices(QueueFlags requiredQueueFlags, bool requirePresent = false, QueueFlags requiredSeparateQueues = {});

	// void CreateCommandBuffers(std::vector<Command>& commands);
	// void DestroyCommandBuffers(std::vector<Command>& commands);

	void AcquireStagingBuffer();
	void ReleaseStagingBuffer();

};
static Context _ctx;

struct DeleteCopyMove {
	DeleteCopyMove() = default;
	DeleteCopyMove(const DeleteCopyMove&) = delete;
	DeleteCopyMove& operator=(const DeleteCopyMove&) = delete;
	DeleteCopyMove(DeleteCopyMove&&) = default;
	DeleteCopyMove& operator=(DeleteCopyMove&&) = default;
	virtual ~DeleteCopyMove() = default;
};

struct Instance: DeleteCopyMove {
	VkInstance handle = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
	std::string applicationName = "Vulkan Slang Compute";
	std::string engineName = "Vulkan Compute";

	uint32_t apiVersion;

	std::vector<bool> activeLayers; // Available layers
	std::vector<const char*> activeLayersNames;
	std::vector<VkLayerProperties> layers;

	std::vector<bool> activeExtensions;                    // Instance Extensions
	std::vector<const char*> activeExtensionsNames;	       // Instance Extensions
	std::vector<VkExtensionProperties> instanceExtensions; // Instance Extensions

	void Create();
	void Destroy();

};

struct PhysicalDevice: DeleteCopyMove {

	UID uid;
	VkPhysicalDevice handle = VK_NULL_HANDLE;
	VkSampleCountFlagBits maxSamples = VK_SAMPLE_COUNT_1_BIT;
	VkSampleCountFlagBits numSamples  = VK_SAMPLE_COUNT_1_BIT;
	VkSampleCountFlags sampleCounts;

	bool descriptorIndexingAvailable = false;
	
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{};
	VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
	VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT graphicsPipelineLibraryFeatures{};
	VkPhysicalDeviceFeatures2 physicalFeatures2{};

	VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT graphicsPipelineLibraryProperties{};

	VkPhysicalDeviceProperties2 physicalProperties2;
	// VkPhysicalDeviceImageFormatInfo2 imageFormatInfo2{};
	// VkImageFormatProperties2 imageFormatProperties2{};

	VkPhysicalDeviceMemoryProperties memoryProperties;
	
	std::vector<VkExtensionProperties> availableExtensions; // Physical DeviceResource Extensions
	std::vector<VkQueueFamilyProperties> availableFamilies;
	
	PhysicalDevice(UID uid, VkPhysicalDevice device): uid(uid), handle(device) {}
	void GetDetails();
	// @param desiredFlags: required queue flags (strict)
	// @param undesiredFlags: undesired queue flags (strict)
	// @param avoidIfPossible: vector of pairs (flags, priority to avoid)
	// @param surfaceToSupport: surface to support (strict)
	// @return best queue family index or QUEUE_NOT_FOUND if strict requirements not met
	std::vector<std::string_view> GetSupportedExtensions(const std::span<const char*>& desiredExtensions);
	uint32_t GetQueueFamilyIndex(VkQueueFlags desiredFlags, VkQueueFlags undesiredFlags, const std::span<const std::pair<VkQueueFlags, float>>& avoidIfPossible = {}, VkSurfaceKHR surfaceToSupport = VK_NULL_HANDLE, const std::span<const uint32_t>& filledUpFamilies = {});
};

struct QueueResource {
	uint32_t familyIndex = ~0;
	uint32_t index = 0;
	VkQueue queue = VK_NULL_HANDLE;
	Command command {};
	// UID uid;
};

struct CommandResource {
	VkDevice device = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	// DeviceResource* device = nullptr;
	// uint32_t queueFamily = ~0;
	QueueResource* queue = nullptr;
	PipelineResource* currentPipeline = nullptr;

	VkCommandPool pool = VK_NULL_HANDLE;
	VkCommandBuffer buffer = VK_NULL_HANDLE;
	VkFence fence = VK_NULL_HANDLE;
	VkQueryPool queryPool;
	std::vector<std::string> timeStampNames;
	std::vector<uint64_t> timeStamps;

	void Create(DeviceResource* device, QueueResource* queue);
	~CommandResource() {
		vkDestroyCommandPool(device, pool, _ctx.allocator);
		vkDestroyFence(device, fence, _ctx.allocator);
		// vkDestroyQueryPool(device, queryPool, _ctx.allocator);
	}
};

struct DeviceResource: DeleteCopyMove {
	UID uid;
	DeviceResource(UID uid): uid(uid){}

	void CreatePipelineImpl(const PipelineDesc& desc, Pipeline& pipeline);

	VkDevice handle = VK_NULL_HANDLE;
	VmaAllocator vmaAllocator;
	PhysicalDevice* physicalDevice = nullptr;

	std::unordered_map<uint32_t, std::shared_ptr<QueueResource>> uniqueQueues;

	// std::unordered_map<uint32_t, QueueResource> uniqueQueues;
	// Owns all command resources outside swapchains
	// Do not resize after creation!
	// std::vector<Command> queuesCommands;
	// QueueResource* queues[QueueFlagBits::Count]; // Pointers to uniqueQueues

	// bindless resources
	VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet bindlessDescriptorSet = VK_NULL_HANDLE;
	VkDescriptorPool bindlessDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout bindlessDescriptorLayout = VK_NULL_HANDLE;

	VkDescriptorPool      descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet       descriptorSet = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

	std::vector<int32_t> availableBufferRID;
	std::vector<int32_t> availableImageRID;
	std::vector<int32_t> availableTLASRID;
	VkSampler genericSampler;

	VkPipelineCache pipelineCache = VK_NULL_HANDLE;

	struct {
		std::unordered_map<UID, VkPipeline> vertexInputInterface;
		std::unordered_map<UID, VkPipeline> preRasterizationShaders;
		std::unordered_map<UID, VkPipeline> fragmentOutputInterface;
		std::vector<VkPipeline> fragmentShaders;
	} pipelineLibrary;
	
	const uint32_t stagingBufferSize = 2 * 1024 * 1024;
	u8* stagingCpu = nullptr;
	uint32_t stagingOffset = 0;
	Buffer staging;
	
	std::vector<const char*> requiredExtensions = { // Physical DeviceResource Extensions
		// VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
		// VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		// VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		// VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		// VK_KHR_RAY_QUERY_EXTENSION_NAME,
		// VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
	};

	void Create(const std::vector<Queue*>& queues);
	void Destroy();
		
	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSet();
	void createDescriptorResources(); // ALL:pool+layout+set
	void destroyDescriptorResources();

	void CreateBindlessDescriptorResources();
	void DestroyBindlessDescriptorResources();

	void CreateVertexInputInterface(const PipelineDesc& desc, Pipeline& pipeline);
	void CreatePreRasterizationShaders(const PipelineDesc& desc, Pipeline& pipeline);
	void CreateFragmentShader(const PipelineDesc& desc, Pipeline& pipeline);
	void CreateFragmentOutputInterface(const PipelineDesc& desc, Pipeline& pipeline);
	void LinkPipeline(Pipeline& pipeline);

	VkSampler CreateSampler(VkDevice device, f32 maxLod);

	void SetDebugUtilsObjectNameEXT(VkDebugUtilsObjectNameInfoEXT* pNameInfo);
	
	PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;
	// PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
	// PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
	// PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
	// PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
	// PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
	// PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;

	~DeviceResource() {
		// dummyVertexBuffer = {};
		// asScratchBuffer = {};
		// vkDestroyDescriptorPool(handle, imguiDescriptorPool, _ctx.allocator);
		staging = {};
		stagingCpu = nullptr;
		DestroyBindlessDescriptorResources();
		vmaDestroyAllocator(vmaAllocator);
		vkDestroySampler(handle, genericSampler, _ctx.allocator);
		// for (auto& pipeline : pipelineLibrary.vertexInputInterface) {
			
		// }
		for (auto& [_, queue] : uniqueQueues) {
			queue->command.resource.reset();
		}
		vkDestroyDevice(handle, _ctx.allocator);
		DEBUG_TRACE("Destroyed logical device");
		handle = VK_NULL_HANDLE;
	}
};


struct SwapChainResource: DeleteCopyMove {
	DeviceResource* device = nullptr;
	// VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;	
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> ImageResources;
	std::vector<VkImageView> ImageViews;
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

	uint32_t framesInFlight = 2;
	uint32_t additionalImages = 0;

	void CreateSurfaceFormats();

	void Create(std::vector<Image>& swapChainImages, uint32_t width, uint32_t height, bool is_recreation = false);
	void Destroy(bool is_recreation = false);

	inline VkSemaphore GetImageAvailableSemaphore(uint32_t currentFrame) { return imageAvailableSemaphores[currentFrame]; }
	inline VkSemaphore GetRenderFinishedSemaphore(uint32_t currentFrame) { return renderFinishedSemaphores[currentFrame]; }
	bool SupportFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);
	void ChooseExtent(uint32_t width, uint32_t height);
	void ChoosePresentMode();
	void ChooseSurfaceFormat();
};


struct Resource {
	DeviceResource* device = nullptr;
	std::string name;
	int32_t rid = -1;
	// Resource(DeviceResource* device, const std::string& name) : device(device), name(name) {}
	virtual ~Resource() {};
};

struct BufferResource : Resource {
	VkBuffer buffer;
	VmaAllocation allocation;

	virtual ~BufferResource() {
		vmaDestroyBuffer(device->vmaAllocator, buffer, allocation);
		// printf("destroy buffer %s\n", name.c_str());
		if (rid >= 0) {
			device->availableBufferRID.push_back(rid);
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
				vkDestroyImageView(device->handle, layerView, _ctx.allocator);
			}
			layersView.clear();
			vkDestroyImageView(device->handle, view, _ctx.allocator);
			vmaDestroyImage(device->vmaAllocator, image, allocation);
			if (rid >= 0) {
				device->availableImageRID.push_back(rid);
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
		vkDestroyPipeline(device->handle, pipeline, _ctx.allocator);
		vkDestroyPipelineLayout(device->handle, layout, _ctx.allocator);
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


void Init(bool requestPresent) {
	_ctx.presentRequested = requestPresent;
	_ctx.instance = std::make_shared<Instance>();
	_ctx.instance->Create();	
	_ctx.GetPhysicalDevices();
}

Device CreateDevice(const std::vector<Queue*>& queues) {
	auto uid = UIDGenerator::Next();
	Device device;
	device.resource = std::make_shared<DeviceResource>(uid);
	auto res = _ctx.devices.try_emplace(uid, device.resource);
	auto& resource = *device.resource;
	resource.Create(queues);
	for (auto& [key, q]: resource.uniqueQueues) {
		q->command.resource->Create(&resource, q.get());
	}
	resource.CreateBindlessDescriptorResources();
	resource.staging = device.CreateBuffer(resource.stagingBufferSize, BufferUsage::TransferSrc, Memory::CPU, "StagingBuffer[" + std::to_string(uid) + "]");
	resource.stagingCpu = (u8*)resource.staging.resource->allocation->GetMappedData();
	return device;
}


void Destroy() {
	// ImGui_ImplVulkan_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	_ctx.physicalDevices.clear();
	_ctx.devices.clear();
	_ctx.instance->Destroy();
}

void* Device::MapBuffer(Buffer& buffer) {
	ASSERT(buffer.memory & Memory::CPU, "Buffer not cpu accessible!");
	void* data;
	vmaMapMemory(resource->vmaAllocator, buffer.resource->allocation, &data);
	return buffer.resource->allocation->GetMappedData();
}

void Device::UnmapBuffer(Buffer& buffer) {
	ASSERT(buffer.memory & Memory::CPU, "Buffer not cpu accessible!");
	vmaUnmapMemory(resource->vmaAllocator, buffer.resource->allocation);
}

Command& Device::GetCommandBuffer(const Queue& queue){
	return queue.resource->command;
}


Buffer Device::CreateBuffer(uint32_t size, BufferUsageFlags usage, MemoryFlags memory, const std::string& name) {
	if (usage & BufferUsage::Vertex) {
		usage |= BufferUsage::TransferDst;
	}

	if (usage & BufferUsage::Index) {
		usage |= BufferUsage::TransferDst;
	}

	if (usage & BufferUsage::Storage) {
		usage |= BufferUsage::Address;
		size += size % resource->physicalDevice->physicalProperties2.properties.limits.minStorageBufferOffsetAlignment;
	}

	if (usage & BufferUsage::AccelerationStructureInput) {
		usage |= BufferUsage::Address;
		usage |= BufferUsage::TransferDst;
	}

	if (usage & BufferUsage::AccelerationStructure) {
		usage |= BufferUsage::Address;
	}

	std::shared_ptr<BufferResource> res = std::make_shared<BufferResource>();
	res->device = this->resource.get();
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
	auto result = vmaCreateBuffer(resource->vmaAllocator, &bufferInfo, &allocInfo, &res->buffer, &res->allocation, nullptr);
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
		res->rid = resource->availableBufferRID.back(); // TODO test: give RID starting from 0, not from end
		resource->availableBufferRID.pop_back();

		VkDescriptorBufferInfo descriptorInfo = {};
		descriptorInfo.buffer = res->buffer;
		descriptorInfo.offset = 0;
		descriptorInfo.range  = size;

		VkWriteDescriptorSet write = {};
		write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet          = resource->bindlessDescriptorSet;
		// write.dstSet          = descriptorSet;
		write.dstBinding      = BINDING_BUFFER;
		write.dstArrayElement = buffer.RID();
		write.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo     = &descriptorInfo;
		vkUpdateDescriptorSets(resource->handle, 1, &write, 0, nullptr);
	}

	return buffer;
}

Image Device::CreateImage(const ImageDesc& desc) {
	std::shared_ptr<ImageResource> res = std::make_shared<ImageResource>();
	res->device = this->resource.get();
	res->name = desc.name;

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
	if (desc.usage & ImageUsage::TransientAttachment) {
		allocInfo.preferredFlags = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
	}
	auto result = vmaCreateImage(resource->vmaAllocator, &imageInfo, &allocInfo, &res->image, &res->allocation, nullptr);
	DEBUG_VK(result, "Failed to create image!");
	// LOG_TRACE("Created image {}", desc.name);

	AspectFlags aspect{};
	switch (desc.format) {
		case D24_unorm_S8_uint:
			aspect = Aspect::Stencil; // Invalid, cannot be both stencil and depth, todo: create separate imageview
		case D32_sfloat:
		case D16_unorm:
			aspect |= Aspect::Depth;
			break;
		
		default:
			aspect = Aspect::Color;
			break;
	}

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
	result = vkCreateImageView(resource->handle, &viewInfo, _ctx.allocator, &res->view);
	DEBUG_VK(result, "Failed to create image view!");
	ASSERT(result == VK_SUCCESS, "Failed to create image view!");

	if (desc.layers > 1) {
		viewInfo.subresourceRange.layerCount = 1;
		res->layersView.resize(desc.layers);
		for (int i = 0; i < desc.layers; i++) {
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.subresourceRange.baseArrayLayer = i;
			result = vkCreateImageView(resource->handle, &viewInfo, _ctx.allocator, &res->layersView[i]);
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
		res->rid = resource->availableImageRID.back();
		resource->availableImageRID.pop_back();
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
			.sampler = resource->genericSampler,
			.imageView = res->view,
			.imageLayout = (VkImageLayout)newLayout,
		};
		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = resource->bindlessDescriptorSet;
		write.dstBinding = BINDING_TEXTURE;
		write.dstArrayElement = image.RID();
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.descriptorCount = 1;
		write.pImageInfo = &descriptorInfo;
		vkUpdateDescriptorSets(resource->handle, 1, &write, 0, nullptr);
	}
	if (desc.usage & ImageUsage::Storage) {
		VkDescriptorImageInfo descriptorInfo = {
			.sampler = resource->genericSampler,
			.imageView = res->view,
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
		};
		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = resource->bindlessDescriptorSet;
		write.dstBinding = BINDING_STORAGE_IMAGE;
		write.dstArrayElement = image.RID();
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.descriptorCount = 1;
		write.pImageInfo = &descriptorInfo;
		vkUpdateDescriptorSets(resource->handle, 1, &write, 0, nullptr);
	}

	VkDebugUtilsObjectNameInfoEXT name = {
	.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
	.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE,
	.objectHandle = (uint64_t)(VkImage)res->image,
	.pObjectName = desc.name.c_str(),
	};
	resource->SetDebugUtilsObjectNameEXT(&name);
	std::string strName = desc.name + "View";
	name = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE_VIEW,
		.objectHandle = (uint64_t)(VkImageView)res->view,
		.pObjectName = strName.c_str(),
	};
	resource->SetDebugUtilsObjectNameEXT(&name);

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
		getchar();
	}

	return ReadBinaryFile(outpath);
}

Pipeline Device::CreatePipeline(const PipelineDesc& desc) {// External handle
	Pipeline pipeline;
	pipeline.resource = std::make_shared<PipelineResource>();
	pipeline.resource->device = this->resource.get();
	pipeline.resource->name = desc.name;
	pipeline.stages = desc.stages;
	_ctx.LoadShaders(pipeline); // load into pipeline
	resource->CreatePipelineImpl(desc, pipeline);
	return pipeline;
}

void DeviceResource::CreatePipelineImpl(const PipelineDesc& desc, Pipeline& pipeline) {
	pipeline.point = desc.point; // Graphics or Compute

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(desc.stages.size());
	std::vector<VkShaderModule> shaderModules(desc.stages.size());
	for (int i = 0; i < desc.stages.size(); i++) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = pipeline.stageBytes[i].size();
		createInfo.pCode = (const uint32_t*)(pipeline.stageBytes[i].data());
		auto result = vkCreateShaderModule(handle, &createInfo, _ctx.allocator, &shaderModules[i]);
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
	pushConstant.size = physicalDevice->physicalProperties2.properties.limits.maxPushConstantsSize;
	pushConstant.stageFlags = VK_SHADER_STAGE_ALL;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = layouts.size();
	pipelineLayoutInfo.pSetLayouts = layouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

	auto vkRes = vkCreatePipelineLayout(handle, &pipelineLayoutInfo, _ctx.allocator, &pipeline.resource->layout);
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

		vkRes = vkCreateComputePipelines(handle, VK_NULL_HANDLE, 1, &pipelineInfo, _ctx.allocator, &pipeline.resource->pipeline);

		DEBUG_VK(vkRes, "Failed to create compute pipeline!");
		ASSERT(vkRes == VK_SUCCESS, "Failed to create compute pipeline!");
	} else {
		// graphics pipeline
		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		// fragments beyond near and far planes are clamped to them
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		// line thickness in terms of number of fragments
		rasterizationState.lineWidth = 1.0f;
		rasterizationState.cullMode = (VkCullModeFlags)desc.cullMode;
		rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.depthBiasConstantFactor = 0.0f;
		rasterizationState.depthBiasClamp = 0.0f;
		rasterizationState.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.minSampleShading = 0.5f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.minDepthBounds = 0.0f;
		depthStencilState.maxDepthBounds = 1.0f;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = {};
		depthStencilState.back = {};

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
		pipelineInfo.pRasterizationState = &rasterizationState;
		pipelineInfo.pMultisampleState = &multisampleState;
		pipelineInfo.pDepthStencilState = &depthStencilState;
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

		vkRes = vkCreateGraphicsPipelines(handle, VK_NULL_HANDLE, 1, &pipelineInfo, _ctx.allocator, &pipeline.resource->pipeline);
		DEBUG_VK(vkRes, "Failed to create graphics pipeline!");
	}

	for (int i = 0; i < shaderModules.size(); i++) {
		vkDestroyShaderModule(handle, shaderModules[i], _ctx.allocator);
	}
}
/* 
namespace GraphicsPipelineLibraryFlags {
	enum : Flags {
		VertexInputInterface = 0x00000001,
		PreRasterizationShaders = 0x00000002,
		FragmentShader = 0x00000004,
		FragmentOutputInterface = 0x00000008,
		MaxEnum = 0x7FFFFFFF
	};
}

Hash64 HashFromFormats(std::vector<int> const& vec) {
  Hash64 seed = vec.size();
  for(auto x : vec) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  return seed;
}

void DeviceResource::CreateVertexInputInterface(const PipelineDesc& desc, Pipeline& pipeline) {
	VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{};
	libraryInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
	libraryInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	if (desc.lineTopology) {
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	} else {
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
	// with this parameter true we can break up lines and triangles in _STRIP topology modes
	inputAssembly.primitiveRestartEnable = VK_FALSE;

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
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)(attributeDescs.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescs.data();

	VkGraphicsPipelineCreateInfo pipelineLibraryInfo{};
	pipelineLibraryInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineLibraryInfo.flags               = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT;
	pipelineLibraryInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineLibraryInfo.pNext               = &libraryInfo;
	pipelineLibraryInfo.pInputAssemblyState = &inputAssembly;
	pipelineLibraryInfo.pVertexInputState   = &vertexInputInfo;
	
	auto res = vkCreateGraphicsPipelines(handle, pipelineCache, 1, &pipelineLibraryInfo, _ctx.allocator, &pipelineLibrary.vertexInputInterface);
	DEBUG_VK(res, "Failed to create vertex input interface!");
}

void DeviceResource::CreatePreRasterizationShaders(const PipelineDesc& desc, Pipeline& pipeline) {
	VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{};
	libraryInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
	libraryInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	if (desc.lineTopology) {
		dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
	}


	VkPipelineDynamicStateCreateInfo dynamicInfo{};
	dynamicInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicInfo.pDynamicStates    = dynamicStates.data();
	dynamicInfo.dynamicStateCount = dynamicStates.size();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType           = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount   = 1;
	viewportState.pViewports      = nullptr;
	viewportState.scissorCount    = 1;
	viewportState.pScissors       = nullptr;

	VkPipelineRasterizationStateCreateInfo rasterizationState = {};
	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	// fragments beyond near and far planes are clamped to them
	rasterizationState.depthClampEnable = VK_FALSE;
	rasterizationState.rasterizerDiscardEnable = VK_FALSE;
	rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	// line thickness in terms of number of fragments
	rasterizationState.lineWidth = 1.0f;
	rasterizationState.cullMode = (VkCullModeFlags)desc.cullMode;
	rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationState.depthBiasEnable = VK_FALSE;
	rasterizationState.depthBiasConstantFactor = 0.0f;
	rasterizationState.depthBiasClamp = 0.0f;
	rasterizationState.depthBiasSlopeFactor = 0.0f;

	// Using the pipeline library extension, we can skip the pipeline shader module creation and directly pass the shader code to the pipeline
	std::vector<uint32_t> spirv;
	load_shader("shared.vert", VK_SHADER_STAGE_VERTEX_BIT, spirv);

	VkShaderModuleCreateInfo shaderModuleInfo{};
	shaderModuleInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleInfo.codeSize = spirv.size() * sizeof(uint32_t);
	shaderModuleInfo.pCode    = spirv.data();

	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.pNext = &shaderModuleInfo;
	shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageInfo.pName = "main";

	VkGraphicsPipelineCreateInfo pipelineLibraryInfo{};
	pipelineLibraryInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineLibraryInfo.pNext               = &libraryInfo;
	pipelineLibraryInfo.renderPass          = VK_NULL_HANDLE;
	pipelineLibraryInfo.flags               = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT;
	pipelineLibraryInfo.stageCount          = 1;
	pipelineLibraryInfo.pStages             = &shaderStageInfo;
	pipelineLibraryInfo.layout              = pipeline.resource->layout;
	pipelineLibraryInfo.pDynamicState       = &dynamicInfo;
	pipelineLibraryInfo.pViewportState      = &viewportState;
	pipelineLibraryInfo.pRasterizationState = &rasterizationState;

	auto res = vkCreateGraphicsPipelines(handle, pipelineCache, 1, &pipelineLibraryInfo, nullptr, &pipeline_library.pre_rasterization_shaders);
	DEBUG_VK(res, "Failed to create vertex input interface!");
}

void DeviceResource::CreateFragmentShader(const PipelineDesc& desc, Pipeline& pipeline) {
	VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{};
	libraryInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
	libraryInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT;

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.front = {},
		.back = {},
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f,
	};

	VkPipelineMultisampleStateCreateInfo multisampleState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 0.5f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
	};

	std::vector<uint32_t> spirv;
	load_shader("uber.frag", VK_SHADER_STAGE_FRAGMENT_BIT, spirv);

	VkShaderModuleCreateInfo shader_module_create_info{};
	shader_module_create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_create_info.codeSize = spirv.size() * sizeof(uint32_t);
	shader_module_create_info.pCode    = spirv.data();

	VkPipelineShaderStageCreateInfo shader_Stage_create_info{};
	shader_Stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_Stage_create_info.pNext = &shader_module_create_info;
	shader_Stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shader_Stage_create_info.pName = "main";

	VkGraphicsPipelineCreateInfo pipelineLibraryInfo = {
		.sType              = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext              = &libraryInfo,
		.flags              = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
		.stageCount         = 1,
		.pStages            = &shader_Stage_create_info,
		.pMultisampleState  = &multisampleState,
		.pDepthStencilState = &depthStencilState,
		.layout             = pipeline.resource->layout,
	};

	VkPipeline fragment_shader = VK_NULL_HANDLE;
	auto res = vkCreateGraphicsPipelines(handle, pipelineCache, 1, &pipelineLibraryInfo, nullptr, &fragment_shader); //todo: Thread pipeline cache
	DEBUG_VK(res, "Failed to create vertex input interface!");

}

void DeviceResource::CreateFragmentOutputInterface(const PipelineDesc& desc, Pipeline& pipeline) {
	VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{};
	libraryInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
	libraryInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT;

	std::vector<VkPipelineColorBlendAttachmentState> blendAttachments(desc.colorFormats.size());
	for (int i = 0; i < desc.colorFormats.size(); i++) {
		blendAttachments[i].colorWriteMask  = VK_COLOR_COMPONENT_R_BIT;
		blendAttachments[i].colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
		blendAttachments[i].colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
		blendAttachments[i].colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
		blendAttachments[i].blendEnable     = VK_FALSE;
	}

	VkPipelineColorBlendStateCreateInfo colorBlendState{};
	colorBlendState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.logicOpEnable           = VK_FALSE;
	colorBlendState.logicOp                 = VK_LOGIC_OP_COPY;
	colorBlendState.attachmentCount         = blendAttachments.size();
	colorBlendState.pAttachments            = blendAttachments.data();
	colorBlendState.blendConstants[0]       = 0.0f;
	colorBlendState.blendConstants[1]       = 0.0f;
	colorBlendState.blendConstants[2]       = 0.0f;
	colorBlendState.blendConstants[3]       = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampleState = {};
	multisampleState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT;
	multisampleState.sampleShadingEnable    = VK_FALSE;
	multisampleState.minSampleShading       = 0.5f;
	multisampleState.pSampleMask            = nullptr;
	multisampleState.alphaToCoverageEnable  = VK_FALSE;
	multisampleState.alphaToOneEnable       = VK_FALSE;

	VkGraphicsPipelineCreateInfo pipelineLibraryInfo{};
	pipelineLibraryInfo.sType                      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineLibraryInfo.pNext                      = &libraryInfo;
	pipelineLibraryInfo.layout                     = pipeline.resource->layout;
	pipelineLibraryInfo.flags                      = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT;
	pipelineLibraryInfo.pColorBlendState           = &colorBlendState;
	pipelineLibraryInfo.pMultisampleState          = &multisampleState;

	auto res = vkCreateGraphicsPipelines(handle, pipelineCache, 1, &pipelineLibraryInfo, nullptr, &pipeline_library.fragment_output_interface);
	DEBUG_VK(res, "Failed to create fragment output interface!");
}

void DeviceResource::LinkPipeline(Pipeline& pipeline) {
	// Create the pipeline using the pre-built pipeline library parts
	// Except for above fragment shader part all parts have been pre-built and will be re-used
	std::vector<VkPipeline> libraries = {
	    pipeline_library.vertex_input_interface,
	    pipeline_library.pre_rasterization_shaders,
	    fragment_shader,
	    pipeline_library.fragment_output_interface
	};

	// Link the library parts into a graphics pipeline
	VkPipelineLibraryCreateInfoKHR linkingInfo{};
	linkingInfo.sType        = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;
	linkingInfo.libraryCount = libraries.size();
	linkingInfo.pLibraries   = libraries.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext      = &linkingInfo;
	pipelineInfo.layout     = pipeline.resource->layout;

	if (_ctx.linkTimeOptimization) {
		pipelineInfo.flags = VK_PIPELINE_CREATE_LINK_TIME_OPTIMIZATION_BIT_EXT;
	}

	VkPipeline executable = VK_NULL_HANDLE;
	auto res = vkCreateGraphicsPipelines(handle, pipelineCache, 1, &pipelineInfo, nullptr, &executable); //todo: Thread pipeline cache
	DEBUG_VK(res, "Failed to create vertex input interface!");

	// pipelines.push_back(executable);

	// Add the fragment shader we created to a deletion list
	pipeline_library.fragment_shaders.push_back(fragment_shader);
}

 */
void DeviceResource::SetDebugUtilsObjectNameEXT(VkDebugUtilsObjectNameInfoEXT* pNameInfo) {
	if (vkSetDebugUtilsObjectNameEXT) {
		vkSetDebugUtilsObjectNameEXT(handle, pNameInfo);
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
	// renderingInfo.renderArea.extent = { uint32_t(extent.x), uint32_t(extent.y)};
	renderingInfo.renderArea.extent = { uint32_t(viewport.z), uint32_t(viewport.w)};
	renderingInfo.renderArea.offset = { offset.x, offset.y };
	renderingInfo.flags = 0;

	std::vector<VkRenderingAttachmentInfoKHR> colorAttachInfos(colorAttachs.size());
	// VkRenderingAttachmentInfoKHR depthAttachInfo;
	for (int i = 0; i < colorAttachs.size(); i++) {
		colorAttachInfos[i] = {};
		colorAttachInfos[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		colorAttachInfos[i].imageView = colorAttachs[i].resource->view;
		colorAttachInfos[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachInfos[i].resolveMode = VK_RESOLVE_MODE_NONE;
		colorAttachInfos[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// colorAttachInfos[i].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachInfos[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachInfos[i].clearValue.color = { 0.1f, 0.1f, 0.1f, 0.0f };
	}

	renderingInfo.colorAttachmentCount = colorAttachInfos.size();
	renderingInfo.pColorAttachments = colorAttachInfos.data();
	renderingInfo.pDepthAttachment = nullptr;
	renderingInfo.pStencilAttachment = nullptr;

	if (depthAttach.resource) {
		VkRenderingAttachmentInfoKHR depthAttachInfo {};
		depthAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		depthAttachInfo.imageView = depthAttach.resource->view;
		depthAttachInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachInfo.resolveMode = VK_RESOLVE_MODE_NONE;
		depthAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachInfo.storeOp = depthAttach.usage & ImageUsage::TransientAttachment ? VK_ATTACHMENT_STORE_OP_DONT_CARE: VK_ATTACHMENT_STORE_OP_STORE;
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
//     DEBUG_ASSERT(_ctx.currentQueue != QueueFlagBits::Transfer, "Time Stamp not supported in Transfer queue");
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
	vkCmdBindDescriptorSets(resource->buffer, (VkPipelineBindPoint)pipeline.point, pipeline.resource->layout, 0, 1, &resource->descriptorSet, 0, nullptr);
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

// vkWaitForFences + vkResetFences +
// vkResetCommandPool + vkBeginCommandBuffer
void Command::BeginCommandBuffer() {
	vkWaitForFences(resource->device, 1, &resource->fence, VK_TRUE, UINT64_MAX);
	vkResetFences(resource->device, 1, &resource->fence);

	// device.resource->stagingOffset = 0;

	// if (resource->timeStamps.size() > 0) {
	//     vkGetQueryPoolResults(resource->device, resource->queryPool, 0, resource->timeStamps.size(), resource->timeStamps.size() * sizeof(uint64_t), resource->timeStamps.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);
	//     for (int i = 0; i < resource->timeStampNames.size(); i++) {
	//         const uint64_t begin = resource->timeStamps[2 * i];
	//         const uint64_t end = resource->timeStamps[2 * i + 1];
	//         _ctx.timeStampTable[resource->timeStampNames[i]] = float(end - begin) * _ctx.physicalProperties2.limits.timestampPeriod / 1000000.0f;
	//     }
	//     resource->timeStamps.clear();
	//     resource->timeStampNames.clear();
	// }

	// ?VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
	vkResetCommandPool(resource->device, resource->pool, 0); // TODO: check if this is needed
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(resource->buffer, &beginInfo);

	// if (queue != QueueFlagBits::Transfer) {
	//     vkCmdResetQueryPool(resource->buffer, resource->queryPool, 0, _ctx.timeStampPerPool);
	// }
}

void Command::EndCommandBuffer() {
	vkEndCommandBuffer(resource->buffer); 
	resource->currentPipeline = {};
}

void Device::WaitQueue(Queue* queue) {
	auto res = vkQueueWaitIdle(queue->resource->queue);
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

// void DeviceResource::WaitQueue(QueueFlagBits queue) {
// 	// todo: wait on fence
// 	auto res = vkQueueWaitIdle(_ctx.queues[queue]->queue);
// 	DEBUG_VK(res, "Failed to wait idle command buffer");
// }


void Device::WaitIdle() {
	auto res = vkDeviceWaitIdle(resource->handle);
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

void Instance::Create(){
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
	if (_ctx.enableValidationLayers) {
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
	
	
	uint32_t glfwExtensionCount = 0;
	if (_ctx.presentRequested) {
		// get all extensions required by glfw
		glfwInit();
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		_ctx.requiredInstanceExtensions.insert(_ctx.requiredInstanceExtensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);
	}

	// Extensions
	if (_ctx.enableValidationLayers) {
		_ctx.requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		if (_ctx.enableDebugReport) {
			_ctx.requiredInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
	}

	// get all available extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 0);
	instanceExtensions.resize(extensionCount);
	activeExtensions.resize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

	// set to active all extensions that we enabled //TODO: Rewrite loop
	for (size_t i = 0; i < _ctx.requiredInstanceExtensions.size(); i++) {
		for (size_t j = 0; j < extensionCount; j++) {
			if (strcmp(_ctx.requiredInstanceExtensions[i], instanceExtensions[j].extensionName) == 0) {
				activeExtensions[j] = true;
				break;
			}
		}
	}

	// Enable validation features if available
	bool validation_features = false;
	if (_ctx.enableValidationLayers){
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
	createInfo.enabledExtensionCount = activeExtensionsNames.size();
	createInfo.ppEnabledExtensionNames = activeExtensionsNames.data();

	

	// which validation layers we need
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (_ctx.enableValidationLayers) {
		createInfo.enabledLayerCount = activeLayersNames.size();
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
	auto res = vkCreateInstance(&createInfo, _ctx.allocator, &handle);
	// LOG_DEBUG("Enabled extensions count: {}", createInfo.enabledExtensionCount);
	// if (createInfo.enabledExtensionCount > 0)
	// 	LOG_DEBUG("Enabled extensions: {}", createInfo.ppEnabledExtensionNames[0]);
	// auto res = VK_SUCCESS;
	DEBUG_VK(res, "Failed to create Vulkan instance!");
	DEBUG_TRACE("Created instance.");
	
	// Debug Utils
	if (_ctx.enableValidationLayers) {
		VkDebugUtilsMessengerCreateInfoEXT messengerInfo;
		PopulateDebugMessengerCreateInfo(messengerInfo);
		res = CreateDebugUtilsMessengerEXT(handle, &messengerInfo, _ctx.allocator, &debugMessenger);
		DEBUG_VK(res, "Failed to set up debug messenger!");
		DEBUG_TRACE("Created debug messenger.");
	}

	// Debug Report
	if (_ctx.enableValidationLayers && _ctx.enableDebugReport) {
		VkDebugReportCallbackCreateInfoEXT debugReportInfo;
		PopulateDebugReportCreateInfo(debugReportInfo);
		// Create the callback handle
		res = CreateDebugReportCallbackEXT(handle, &debugReportInfo, nullptr, &debugReport);
		// DEBUG_VK(res, "Failed to set up debug report callback!");
		DEBUG_TRACE("Created debug report callback.");
	}

	
	LOG_INFO("Created VulkanInstance.");
}

void Instance::Destroy() {
	activeLayersNames.clear();
	activeExtensionsNames.clear();
	if (debugMessenger) {
		DestroyDebugUtilsMessengerEXT(_ctx.instance->handle, debugMessenger, _ctx.allocator);
		DEBUG_TRACE("Destroyed debug messenger.");
		debugMessenger = nullptr;
	}
	if (debugReport) {
		DestroyDebugReportCallbackEXT(_ctx.instance->handle, debugReport, _ctx.allocator);
		DEBUG_TRACE("Destroyed debug report callback.");
		debugReport = nullptr;
	}

	vkDestroyInstance(_ctx.instance->handle, _ctx.allocator);
	DEBUG_TRACE("Destroyed instance.");
}

void Context::GetPhysicalDevices() {
	// get all devices with Vulkan support
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(_ctx.instance->handle, &count, nullptr);
	ASSERT(count != 0, "no GPUs with Vulkan support!");
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(_ctx.instance->handle, &count, devices.data());
	DEBUG_TRACE("Found {0} physical device(s).", count);
	_ctx.physicalDevices.reserve(count);
	for (const auto& device: devices) {
		// device->physicalDevices->handle.insert({UIDGenerator::Next(), PhysicalDevice()});
		auto uid = UIDGenerator::Next();
		auto res = _ctx.physicalDevices.try_emplace(uid, PhysicalDevice(uid, device));
		auto& physicalDevice = res.first->second;
		physicalDevice.GetDetails();
	};
}

void PhysicalDevice::GetDetails() {
	// get all available extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionCount, nullptr);
	availableExtensions.resize(extensionCount);
	vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionCount, availableExtensions.data());

	// get all available families
	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(handle, &familyCount, nullptr);
	availableFamilies.resize(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(handle, &familyCount, availableFamilies.data());

	graphicsPipelineLibraryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT;
	graphicsPipelineLibraryFeatures.pNext = nullptr;
	bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	bufferDeviceAddressFeatures.pNext = &graphicsPipelineLibraryFeatures;
	indexingFeatures.sType	= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	indexingFeatures.pNext = &bufferDeviceAddressFeatures;
	physicalFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	physicalFeatures2.pNext = &indexingFeatures;
	vkGetPhysicalDeviceFeatures2(handle, &physicalFeatures2);

	
	graphicsPipelineLibraryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_PROPERTIES_EXT;
	graphicsPipelineLibraryProperties.pNext = nullptr;
	physicalProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	physicalProperties2.pNext = &graphicsPipelineLibraryProperties;
	vkGetPhysicalDeviceProperties2(handle, &physicalProperties2);
	vkGetPhysicalDeviceMemoryProperties(handle, &memoryProperties);
	// imageFormatProperties2.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
	// imageFormatInfo2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
	// vkGetPhysicalDeviceImageFormatProperties2(physicalDevice, &imageFormatInfo2, &imageFormatProperties2);

	VkSampleCountFlags counts = physicalProperties2.properties.limits.framebufferColorSampleCounts;
	counts &= physicalProperties2.properties.limits.framebufferDepthSampleCounts;

	// get max number of samples
	maxSamples = VK_SAMPLE_COUNT_1_BIT;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { maxSamples = VK_SAMPLE_COUNT_64_BIT; }
	else if (counts & VK_SAMPLE_COUNT_32_BIT) { maxSamples = VK_SAMPLE_COUNT_32_BIT; }
	else if (counts & VK_SAMPLE_COUNT_16_BIT) { maxSamples = VK_SAMPLE_COUNT_16_BIT; }
	else if (counts & VK_SAMPLE_COUNT_8_BIT) { maxSamples = VK_SAMPLE_COUNT_8_BIT; }
	else if (counts & VK_SAMPLE_COUNT_4_BIT) { maxSamples = VK_SAMPLE_COUNT_4_BIT; }
	else if (counts & VK_SAMPLE_COUNT_2_BIT) { maxSamples = VK_SAMPLE_COUNT_2_BIT; }

	if (numSamples > maxSamples) {numSamples = maxSamples;}
}

auto PhysicalDevice::GetSupportedExtensions(const std::span<const char*>& desiredExtensions) -> std::vector<std::string_view> {
	std::vector<std::string_view> extensionsToEnable;
	extensionsToEnable.reserve(desiredExtensions.size());
	std::copy_if(desiredExtensions.begin(), desiredExtensions.end(), std::back_inserter(extensionsToEnable),
		[&](const char* desiredExtension) {
			if (std::any_of(availableExtensions.begin(), availableExtensions.end(), [&desiredExtension](const auto& availableExtension) {
					return std::string_view(availableExtension.extensionName) == desiredExtension;
			}) == true) { return true; } else {
				LOG_WARN("Extension {} not supported on device {}", desiredExtension, physicalProperties2.properties.deviceName);
				return false;
			}
		});
	return extensionsToEnable;
}


constexpr uint32_t QUEUE_NOT_FOUND = ~0u;
constexpr uint32_t ALL_SUITABLE_QUEUES_TAKEN = ~0u - 1;

uint32_t PhysicalDevice::GetQueueFamilyIndex(VkQueueFlags desiredFlags, VkQueueFlags undesiredFlags, const std::span<const std::pair<VkQueueFlags, float>>& avoidIfPossible, VkSurfaceKHR surfaceToSupport, const std::span<const uint32_t>& filledUpFamilies) {
	std::vector<std::pair<uint32_t, float>> candidates;
	auto& families = availableFamilies;
	for (auto i = 0; i < families.size(); i++) {
		bool suitable = ((families[i].queueFlags & desiredFlags) == desiredFlags && ((families[i].queueFlags & undesiredFlags) == 0));
		if (surfaceToSupport != VK_NULL_HANDLE) {
			VkBool32 presentSupport = false;
			VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surfaceToSupport, &presentSupport);
			if (res != VK_SUCCESS) {LOG_ERROR("vkGetPhysicalDeviceSurfaceSupportKHR failed with error code: {}", (uint32_t)res) continue;}
			suitable = suitable && presentSupport;
		}
		if (!suitable) continue; 
		if (avoidIfPossible.empty()) return i;
		std::pair<uint32_t, float> candidate(i, 0.0f);
		// Prefer family with least number of VkQueueFlags
		for (VkQueueFlags bit = 1; bit != 0; bit <<= 1) {
			if (families[i].queueFlags & bit) { candidate.second += 0.01f; }
		}
		// Add weight for supporting unwanted VkQueueFlags
		for (auto& avoid: avoidIfPossible) {
			if ((families[i].queueFlags & avoid.first) == avoid.first) {
				candidate.second += avoid.second;
			}
		}
		// Add candidate if family is not filled up
		if (std::find(filledUpFamilies.begin(), filledUpFamilies.end(), i) == filledUpFamilies.end()) {
			candidates.push_back(candidate);
		} else {
			LOG_WARN("Queue family {} is filled up", i);
		}
	}
	if (candidates.empty()) { return ALL_SUITABLE_QUEUES_TAKEN; }
	// print candidates
	for (auto& c: candidates) {
		DEBUG_TRACE("Candidate: {} ({})", c.first, c.second);
	}
	auto bestFamily = std::min_element(candidates.begin(), candidates.end(), [](auto& l, auto& r) {return l.second < r.second;})->first;
	DEBUG_TRACE("Best family: {}", bestFamily);
	return bestFamily;

}

void DeviceResource::Create(const std::vector<Queue*>& queues) {
	std::unordered_map<uint32_t, uint32_t> queuesToCreate; // family index -> queue count
	std::vector<uint32_t> filledUpFamilies;
	queuesToCreate.reserve(queues.size());
	std::unordered_map<GLFWwindow*, VkSurfaceKHR> surfaces;

	// Create surfaces for checking present support
	for (auto queue: queues) {
		if (queue->supportedWindowToPresent == nullptr) continue;
		VkSurfaceKHR surface;
		auto res = glfwCreateWindowSurface(_ctx.instance->handle, queue->supportedWindowToPresent, _ctx.allocator, &surface);
		if (res != VK_SUCCESS) {
			LOG_WARN("Failed to create surface for window: {}", (void*)queue->supportedWindowToPresent);
		} else {
			surfaces.try_emplace(queue->supportedWindowToPresent, surface);
		}
	}
	if (!surfaces.empty()) {
		requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}
	for (auto& [uid, physicalDevice]: _ctx.physicalDevices) {
		// Require extension support
		if (physicalDevice.GetSupportedExtensions(requiredExtensions).size() < requiredExtensions.size()) {
			continue;
		}
		queuesToCreate.clear();
		filledUpFamilies.clear();
		bool deviceSuitable = true;
		for (auto queue: queues) {
			if (queue->resource) {
				LOG_WARN("Queue already has resource with flags ({}) family ({}) index ({}). Ignoring request for new queue.", (uint32_t)queue->flags, queue->resource->familyIndex, queue->resource->index);
				continue;
			}
			std::span<const std::pair<VkQueueFlags, float>> avoidIfPossible{};
			if(queue->preferSeparateFamily) {
				switch (queue->flags) {
				case VK_QUEUE_COMPUTE_BIT:  avoidIfPossible = {{{VK_QUEUE_GRAPHICS_BIT, 1.0f}, {VK_QUEUE_TRANSFER_BIT, 0.5f}}}; break;
				case VK_QUEUE_TRANSFER_BIT: avoidIfPossible = {{{VK_QUEUE_GRAPHICS_BIT, 1.0f}, {VK_QUEUE_COMPUTE_BIT, 0.5f}}}; break;
				default:                    avoidIfPossible = {{{VK_QUEUE_GRAPHICS_BIT, 1.0f}}}; break;
				}
			}
			auto familyIndex = physicalDevice.GetQueueFamilyIndex(queue->flags, 0, avoidIfPossible, surfaces[queue->supportedWindowToPresent], filledUpFamilies);
			if (familyIndex == QUEUE_NOT_FOUND) {
				LOG_WARN("Requested queue flags ({}) not available on device: ({})", (uint32_t)queue->flags, physicalDevice.physicalProperties2.properties.deviceName);
				deviceSuitable = false;
				break;
			} else if (familyIndex == ALL_SUITABLE_QUEUES_TAKEN) {
				LOG_WARN("Requested more queues with flags ({}) than available on device: ({}). Queue was not created", queue->flags, physicalDevice.physicalProperties2.properties.deviceName);
				continue;
			}
			queue->resource = std::make_shared<QueueResource>(familyIndex, queuesToCreate[familyIndex]);
			queuesToCreate[familyIndex]++;
			if (queuesToCreate[familyIndex] == physicalDevice.availableFamilies[familyIndex].queueCount) {
				filledUpFamilies.push_back(familyIndex);
			}
		
			// }
		}
		if (deviceSuitable) {
			this->physicalDevice = &physicalDevice;
			break;
		}
	}
	if (this->physicalDevice == nullptr) {
		LOG_ERROR("Failed to find suitable device");
		// return;
	}
	ASSERT(this->physicalDevice != nullptr, "Failed to find suitable device");

	// Destroy test surfaces
	for (auto& [window, surface]: surfaces) {
		vkDestroySurfaceKHR(_ctx.instance->handle, surface, _ctx.allocator);
	}

	uint32_t maxQueuesInFamily = std::max_element(physicalDevice->availableFamilies.begin(), physicalDevice->availableFamilies.end(), [](const auto& a, const auto& b) {
		return a.queueCount < b.queueCount;
	}).base()->queueCount;

	// priority for each type of queue (1.0f for all)
	std::vector<float> priorities(maxQueuesInFamily, 1.0f);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto &[family, count]: queuesToCreate) {
		LOG_INFO("Creating queue family {} with {} queues", family, count);
		VkDeviceQueueCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = family;
		createInfo.queueCount = count;
		createInfo.pQueuePriorities = &priorities[0];
		queueCreateInfos.push_back(createInfo);
	}

	auto supportedFeatures = physicalDevice->physicalFeatures2.features;
	
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
	
	VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT graphicsPipelineLibraryFeatures{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT,
		.pNext = nullptr,
		.graphicsPipelineLibrary = physicalDevice->graphicsPipelineLibraryFeatures.graphicsPipelineLibrary,
	};

	// ask for features if available
	// ref: https://dev.to/gasim/implementing-bindless-design-in-vulkan-34no
	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
		.pNext = &graphicsPipelineLibraryFeatures,
		.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE,
		.shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
		.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
		.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
		.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
		.descriptorBindingPartiallyBound = VK_TRUE,
		.runtimeDescriptorArray = VK_TRUE,
	};

	VkPhysicalDeviceBufferDeviceAddressFeatures addresFeatures{};
	addresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	addresFeatures.bufferDeviceAddress = VK_TRUE;
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

	requiredExtensions.insert(requiredExtensions.end(), _ctx.preferredExtensions.begin(), _ctx.preferredExtensions.end());

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	// createInfo.queueCreateInfoCount = 1;
	// createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.enabledExtensionCount = requiredExtensions.size();
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	// createInfo.pEnabledFeatures; // !Should be NULL if pNext is used
	createInfo.pNext = &features2; // feature chain


	// specify the required layers to the device 
	if (_ctx.enableValidationLayers) {
		auto& layers = _ctx.instance->activeLayersNames;
		createInfo.enabledLayerCount = layers.size();
		createInfo.ppEnabledLayerNames = layers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	auto res = vkCreateDevice(physicalDevice->handle, &createInfo, _ctx.allocator, &handle);
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
	allocatorCreateInfo.physicalDevice = physicalDevice->handle;
	allocatorCreateInfo.device = handle;
	allocatorCreateInfo.instance = _ctx.instance->handle;
	allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
	vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator);

	for (auto& q: queues) {
		if (!q->resource) continue; // queue was not created
		auto key = q->resource->familyIndex*maxQueuesInFamily + q->resource->index;
		// auto sh = std::shared_ptr<Queue>(q->resource);
		auto it = uniqueQueues.try_emplace(key, q->resource);
		if (!it.second) {
			LOG_WARN("Queue family index {} index {} already exists", q->resource->familyIndex, q->resource->index);
		}
		vkGetDeviceQueue(handle, q->resource->familyIndex, q->resource->index, &q->resource->queue);
	}

	genericSampler = CreateSampler(handle, 1.0);
	if (_ctx.enableValidationLayers) {
		vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(handle, "vkSetDebugUtilsObjectNameEXT");
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

	// VkResult result = vkCreateDescriptorPool(device, &imguiPoolInfo, _ctx.allocator, &imguiDescriptorPool);
	// DEBUG_VK(result, "Failed to create imgui descriptor pool!");
}




void SwapChainResource::CreateSurfaceFormats() {
	// get capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physicalDevice->handle, surface, &surfaceCapabilities);

	// get surface formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice->handle, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		availableSurfaceFormats.clear();
		availableSurfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice->handle, surface, &formatCount, availableSurfaceFormats.data());
	}

	// get present modes
	uint32_t modeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice->handle, surface, &modeCount, nullptr);
	if (modeCount != 0) {
		availablePresentModes.clear();
		availablePresentModes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice->handle, surface, &modeCount, availablePresentModes.data());
	}

	// set this device as not suitable if no surface formats or present modes available
	bool suitable = (modeCount > 0);
	suitable &= (formatCount > 0);

	if (!suitable) {
		LOG_ERROR("selected device invalidated after surface update.");
	}
}

void SwapChainResource::Create(std::vector<Image>& swapChainImages, uint32_t width, uint32_t height, bool is_recreation) {
	// this->device = handle; // TODO: make this a parameter
	CreateSurfaceFormats();

	// create swapchain
	{
		const auto& capabilities = surfaceCapabilities;
		ChooseSurfaceFormat();
		ChoosePresentMode();
		ChooseExtent(width, height);

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
		createInfo.imageFormat = colorFormat;
		createInfo.imageColorSpace = colorSpace;
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

		auto res = vkCreateSwapchainKHR(device->handle, &createInfo, _ctx.allocator, &swapChain);
		DEBUG_VK(res, "Failed to create swap chain!");
		
		// DEBUG_TRACE("Creating swapchain with {} images.", imageCount);
		vkGetSwapchainImagesKHR(device->handle, swapChain, &imageCount, nullptr);
		ImageResources.resize(imageCount);
		vkGetSwapchainImagesKHR(device->handle, swapChain, &imageCount, ImageResources.data());
		// DEBUG_TRACE("Created swapchain with {} images.", ImageResources.size());
	}

	// create image views
	ImageViews.resize(ImageResources.size());
	swapChainImages.resize(ImageResources.size());
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = ImageResources[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = colorFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		auto res = vkCreateImageView(device->handle, &viewInfo, _ctx.allocator, &ImageViews[i]);
		DEBUG_VK(res, "Failed to create SwapChain image view!");

		swapChainImages[i].resource = std::make_shared<ImageResource>();
		swapChainImages[i].resource->fromSwapchain = true;
		swapChainImages[i].resource->image = ImageResources[i];
		swapChainImages[i].resource->view = ImageViews[i];
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
			.objectHandle = (uint64_t)(VkImage)ImageResources[i],
			.pObjectName = strName.c_str(),
		};
		device->SetDebugUtilsObjectNameEXT(&name);
		swapChainImages[i].resource->name = strName;
		strName = ("SwapChain View " + std::to_string(i));
		name = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE_VIEW,
			.objectHandle = (uint64_t)(VkImageView)ImageViews[i],
			.pObjectName = strName.c_str(),
		};
		device->SetDebugUtilsObjectNameEXT(&name);
	}

	if (!is_recreation)
	{
		// synchronization objects
		imageAvailableSemaphores.resize(framesInFlight);
		renderFinishedSemaphores.resize(framesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (size_t i = 0; i < framesInFlight; i++) {
			auto res = vkCreateSemaphore(device->handle, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
			DEBUG_VK(res, "Failed to create semaphore!");
			res = vkCreateSemaphore(device->handle, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
			DEBUG_VK(res, "Failed to create semaphore!");
		}


	}
}

void SwapChainResource::Destroy(bool is_recreation) {
	for (size_t i = 0; i < ImageViews.size(); i++) {
		vkDestroyImageView(device->handle, ImageViews[i], _ctx.allocator);
	}
	
	if (!is_recreation) {
		for (size_t i = 0; i < framesInFlight; i++) {
			vkDestroySemaphore(device->handle, imageAvailableSemaphores[i], _ctx.allocator);
			vkDestroySemaphore(device->handle, renderFinishedSemaphores[i], _ctx.allocator);
		}
		imageAvailableSemaphores.clear();
		renderFinishedSemaphores.clear();
		availablePresentModes.clear();
		availableSurfaceFormats.clear();
	}

	vkDestroySwapchainKHR(device->handle, swapChain, _ctx.allocator);
	if (!is_recreation) {
		vkDestroySurfaceKHR(_ctx.instance->handle, surface, _ctx.allocator);
	}

	ImageViews.clear();
	swapChain = VK_NULL_HANDLE;
}

void SwapChain::Create(Device& device, vkw::Queue& queue, GLFWwindow* window, uint32_t width, uint32_t height) {
	this->window = window;

	resource = std::make_shared<SwapChainResource>();
	resource->device = device.resource.get();

	VkResult res = glfwCreateWindowSurface(_ctx.instance->handle, window, _ctx.allocator, &resource->surface);
	DEBUG_VK(res, "Failed to create window surface!");
	DEBUG_TRACE("Created surface.");
	resource->Create(swapChainImages, width, height, false);
		// command buffers
	commands.resize(framesInFlight);
	for (auto& cmd: commands) {
		cmd.resource->Create(device.resource.get(), queue.resource.get());
	}
		// LOG_INFO("Created Swapchain {}", _ctx.swapChains.size());
	currentFrame = 0;
	currentImageIndex = 0;
	dirty = false;
}

void SwapChain::Destroy(){
	for (auto& cmd: commands) {
		vkWaitForFences(resource->device->handle, 1, &cmd.resource->fence, VK_TRUE, UINT_MAX);
	}
	commands.clear();
	swapChainImages.clear();
	resource->Destroy();
}

void ImGuiCheckVulkanResult(VkResult res) {
    if (res == 0) {
        return;
	}
	LOG_WARN("Imgui error: {}", VK_ERROR_STRING(res));
	DEBUG_VK(res, "Imgui error: {}", VK_ERROR_STRING(res));
}
/* 
void SwapChain::CreateImGui(GLFWwindow* window) {
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = _ctx.instance->handle;
    initInfo.PhysicalDevice = device->physicalDevice->handle;
    initInfo.DeviceResource = handle;
    initInfo.QueueFamily = _ctx.queues[vkw::QueueFlagBits::Graphics]->familyIndex;
    initInfo.QueueFlagBits = _ctx.queues[vkw::QueueFlagBits::Graphics]->queue;
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
 */
void SwapChain::DestroyImGui() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void SwapChain::Recreate(uint32_t width, uint32_t height) {
	DEBUG_ASSERT(!(width == 0 || height == 0), "Window size is 0, swapchain NOT recreated");
	resource->Destroy(true);
	resource->Create(swapChainImages, width, height, true);
}

bool SwapChain::AcquireImage() {
	auto res = vkAcquireNextImageKHR(resource->device->handle, resource->swapChain, UINT64_MAX, resource->GetImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &currentImageIndex);

	if (res == VK_ERROR_OUT_OF_DATE_KHR) {
		LOG_WARN("AcquireImage: Out of date");
		dirty = true;
	} else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
		DEBUG_VK(res, "Failed to acquire swap chain image!");
	}

	if (res == VK_SUCCESS) {
		return true;
	} else {
		return false;
	}
}

bool SwapChain::GetDirty() {
	return dirty;
}

// EndCommandBuffer + vkQueuePresentKHR
void SwapChain::SubmitAndPresent() {

	SubmitInfo submitInfo{};
	submitInfo.waitSemaphore   = (Semaphore*)resource->GetImageAvailableSemaphore(currentFrame);
	submitInfo.signalSemaphore = (Semaphore*)resource->GetRenderFinishedSemaphore(currentFrame);
	
	auto cmd = GetCommandBuffer();
	cmd.EndCommandBuffer();
	cmd.QueueSubmit(submitInfo);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1; // TODO: pass with info
	presentInfo.pWaitSemaphores = (VkSemaphore*)&submitInfo.signalSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &resource->swapChain;
	presentInfo.pImageIndices = &currentImageIndex;
	presentInfo.pResults = nullptr;

	auto res = vkQueuePresentKHR(cmd.resource->queue->queue, &presentInfo); // TODO: use present queue

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
		if (res == VK_ERROR_OUT_OF_DATE_KHR) { LOG_WARN("vkQueuePresentKHR: Out of date") }
		if (res == VK_SUBOPTIMAL_KHR) { LOG_WARN("vkQueuePresentKHR: Suboptimal") }

		dirty = true;
		return;
	}
	else if (res != VK_SUCCESS) {
		DEBUG_VK(res, "Failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % framesInFlight;
}


const u32 MAX_STORAGE = 64;
const u32 MAX_SAMPLEDIMAGES = 64;
// const u32 MAX_ACCELERATIONSTRUCTURE = 64;
const u32 MAX_STORAGE_IMAGES = 8192;


void DeviceResource::createDescriptorPool(){
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
	VkResult result = vkCreateDescriptorPool(handle, &descriptorPoolCreateInfo, NULL, &descriptorPool);
	DEBUG_VK(result, "Failed to create descriptor pool!");
	ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool!");
}

void DeviceResource::createDescriptorSetLayout(){
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

	VkResult result = vkCreateDescriptorSetLayout(handle, &descriptorSetLayoutCreateInfo, NULL, &descriptorSetLayout);
	DEBUG_VK(result, "Failed to allocate descriptor set!");
	ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor set!");
}

void DeviceResource::createDescriptorSet(){
	// Descriptor Set
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	VkResult result = vkAllocateDescriptorSets(handle, &allocInfo, &descriptorSet);
	DEBUG_VK(result, "Failed to allocate descriptor set!");
	ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor set!");
}

void DeviceResource::createDescriptorResources(){
	createDescriptorSetLayout();
	createDescriptorPool();
	createDescriptorSet();
}

// vkDestroyDescriptorPool + vkDestroyDescriptorSetLayout
void DeviceResource::destroyDescriptorResources(){
	vkDestroyDescriptorPool(handle, descriptorPool, _ctx.allocator);
	vkDestroyDescriptorSetLayout(handle, descriptorSetLayout, _ctx.allocator);
	descriptorSet = VK_NULL_HANDLE;
	descriptorPool = VK_NULL_HANDLE;
	descriptorSetLayout = VK_NULL_HANDLE;
}

void DeviceResource::CreateBindlessDescriptorResources(){
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

		VkResult result = vkCreateDescriptorPool(handle, &bindlessPoolInfo, _ctx.allocator, &bindlessDescriptorPool);
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

		result = vkCreateDescriptorSetLayout(handle, &descriptorLayoutInfo, _ctx.allocator, &bindlessDescriptorLayout);
		DEBUG_VK(result, "Failed to create bindless descriptor set layout!");
		
		// create Descriptor Set for bindless resources
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = bindlessDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &bindlessDescriptorLayout;

		result = vkAllocateDescriptorSets(handle, &allocInfo, &bindlessDescriptorSet);
		DEBUG_VK(result, "Failed to allocate bindless descriptor set!");
		ASSERT(result == VK_SUCCESS, "Failed to allocate bindless descriptor set!");
	}

	// asScratchBuffer = vkw::CreateBuffer(initialScratchBufferSize, vkw::BufferUsage::Address | vkw::BufferUsage::Storage, vkw::Memory::GPU);
	// VkBufferDeviceAddressInfo scratchInfo{};
	// scratchInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	// scratchInfo.buffer = asScratchBuffer.resource->buffer;
	// asScratchAddress = vkGetBufferDeviceAddress(handle, &scratchInfo);

	// dummyVertexBuffer = vkw::CreateBuffer(
	// 	6 * 3 * sizeof(float),
	// 	vkw::BufferUsage::Vertex | vkw::BufferUsage::AccelerationStructureInput,
	// 	vkw::Memory::GPU,
	// 	"VertexBuffer#Dummy"
	// );
}

void DeviceResource::DestroyBindlessDescriptorResources(){
	vkDestroyDescriptorPool(handle, bindlessDescriptorPool, _ctx.allocator);
	vkDestroyDescriptorSetLayout(handle, bindlessDescriptorLayout, _ctx.allocator);
	bindlessDescriptorSet = VK_NULL_HANDLE;
	bindlessDescriptorPool = VK_NULL_HANDLE;
	bindlessDescriptorLayout = VK_NULL_HANDLE;
}


void CommandResource::Create(DeviceResource* device, QueueResource* queue) {
	this->device = device->handle;
	this->queue = queue;
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = 0; // do not use VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
	
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	static int buf_count = 0;

	poolInfo.queueFamilyIndex = this->queue->familyIndex;
	auto res = vkCreateCommandPool(device->handle, &poolInfo, _ctx.allocator, &pool);
	DEBUG_VK(res, "Failed to create command pool!");

	allocInfo.commandPool = pool;
	res = vkAllocateCommandBuffers(device->handle, &allocInfo, &buffer);
	DEBUG_VK(res, "Failed to allocate command buffer!");

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vkCreateFence(device->handle, &fenceInfo, _ctx.allocator, &fence);

	// VkQueryPoolCreateInfo queryPoolInfo{};
	// queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	// queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
	// queryPoolInfo.queryCount = timeStampPerPool;
	// res = vkCreateQueryPool(device->handle, &queryPoolInfo, _ctx.allocator, &queryPool);
	// DEBUG_VK(res, "failed to create query pool");

	// timeStamps.clear();
	// timeStampNames.clear();

}

bool SwapChainResource::SupportFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) {
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(device->physicalDevice->handle, format, &props);

	if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
		return true;
	} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
		return true;
	}

	return false;
}

void SwapChainResource::ChooseSurfaceFormat() {
	for (const auto& availableFormat : this->availableSurfaceFormats) {
		if (availableFormat.format == colorFormat
			&& availableFormat.colorSpace == colorSpace) {
			return;
		}
	}
	LOG_WARN("Preferred surface format not available!");
	this->colorFormat = this->availableSurfaceFormats[0].format;
	this->colorSpace = this->availableSurfaceFormats[0].colorSpace;
}

void SwapChainResource::ChoosePresentMode() {
	for (const auto& mode : this->availablePresentModes) {
		if (mode == presentMode) {
			this->presentMode =  mode;
			return;
		}
	}
	LOG_WARN("Preferred present mode not available!");
	// FIFO is guaranteed to be available
	this->presentMode =  VK_PRESENT_MODE_FIFO_KHR;
}

void SwapChainResource::ChooseExtent(uint32_t width, uint32_t height) {
	if (this->surfaceCapabilities.currentExtent.width != UINT32_MAX) {
		this->extent = this->surfaceCapabilities.currentExtent;
	} else {
		VkExtent2D actualExtent = { width, height };

		actualExtent.width = std::max (
			this->surfaceCapabilities.minImageExtent.width,
			std::min(this->surfaceCapabilities.maxImageExtent.width, actualExtent.width)
		);
		actualExtent.height = std::max (
			this->surfaceCapabilities.minImageExtent.height,
			std::min(this->surfaceCapabilities.maxImageExtent.height, actualExtent.height)
		);
		DEBUG_TRACE("ChooseExtent: {0}, {1}", actualExtent.width, actualExtent.height);
		this->extent = actualExtent;
	}
}



void Command::Copy(Device& device, Buffer& dst, void* data, uint32_t size, uint32_t dstOfsset) {
	if (device.resource->stagingBufferSize - device.resource->stagingOffset < size) {
		LOG_ERROR("not enough size in staging buffer to copy");
		// todo: allocate additional buffer
		return;
	}
	// memcpy(resource->stagingCpu + resource->stagingOffset, data, size);
	vmaCopyMemoryToAllocation(device.resource->vmaAllocator, data, device.resource->staging.resource->allocation, device.resource->stagingOffset, size);
	Copy(dst, device.resource->staging, size, dstOfsset, device.resource->stagingOffset);
	DEBUG_TRACE("CmdCopy, size: {}, offset: {}", size, device.resource->stagingOffset);
	device.resource->stagingOffset += size;
}

void Command::Copy(Buffer& dst, Buffer& src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset) {
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(resource->buffer, src.resource->buffer, dst.resource->buffer, 1, &copyRegion);
}

void Command::Copy(Device& device, Image& dst, void* data, uint32_t size) {
	if (device.resource->stagingBufferSize - device.resource->stagingOffset < size) {
		LOG_ERROR("not enough size in staging buffer to copy");
		// todo: allocate additional buffer
		return;
	}
	memcpy(device.resource->stagingCpu + device.resource->stagingOffset, data, size);
	Copy(dst, device.resource->staging, device.resource->stagingOffset);
	device.resource->stagingOffset += size;
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

VkSampler DeviceResource::CreateSampler(VkDevice device, f32 maxLod) {
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
	auto vkRes = vkCreateSampler(handle, &samplerInfo, nullptr, &sampler);
	DEBUG_VK(vkRes, "Failed to create texture sampler!");
	ASSERT(vkRes == VK_SUCCESS, "Failed to create texture sampler!");

	return sampler;
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