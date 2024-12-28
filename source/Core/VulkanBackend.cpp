#ifdef USE_MODULES
module;
#endif

#include "Bindless.h"

// #define VK_NO_STDDEF_H
#include <vulkan/vulkan_core.h>

#define GLSL_VALIDATOR "glslangValidator"
#define SLANGC "slangc"

#define BIN_PATH "bin"

#define SHADER_ALWAYS_COMPILE 0

#ifdef USE_MODULES
module VulkanBackend;
import vk_mem_alloc;
import Lmath;
import glfw;
import Types;
import Log;
import Util;
import stl;
import imgui;
import imgui_impl_glfw;
import imgui_impl_vulkan;

#else
#include "vk_mem_alloc.cppm"
#include "Lmath.cppm"
#include "VulkanBackend.cppm"
#include "Types.cppm"
#include "Log.cppm"
#include "imgui_impl_vulkan.cppm"
#include "imgui_impl_glfw.cppm"
#include "Util.cppm"
#include "glfw.cppm"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <string_view>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <array>


#endif

#include "Macros.h"

#ifdef NRAY_DEBUG
#define DEBUG_VK(res, ...) { if ((res) != VK_SUCCESS) { LOG_ERROR("[VULKAN ERROR = {0}] {1} in {2}:{3}", VK_ERROR_STRING(res), __VA_ARGS__, __FILE__, __LINE__); exit(1);}}
#else
#define DEBUG_VK(res, ...)
#endif

static const char *VK_ERROR_STRING(VkResult result);
namespace vkw {
struct Instance;
struct PhysicalDevice;
struct DeviceResource;
struct SwapChain;

namespace Cast {

inline constexpr VkFormat VkFormat(enum Format value) {
	switch (value) {
		case Format::Undefined:         return VK_FORMAT_UNDEFINED;

		case Format::R8_unorm:          return VK_FORMAT_R8_UNORM;

		case Format::RGBA8_UNORM:       return VK_FORMAT_R8G8B8A8_UNORM;
		case Format::RGBA8_SRGB:        return VK_FORMAT_R8G8B8A8_SRGB;
		case Format::BGRA8_unorm:       return VK_FORMAT_B8G8R8A8_UNORM;
		case Format::BGRA8_sRGB:        return VK_FORMAT_B8G8R8A8_SRGB;
		case Format::RGBA16_sfloat:     return VK_FORMAT_R16G16B16A16_SFLOAT;

		case Format::RG32_sfloat:       return VK_FORMAT_R32G32_SFLOAT;
		case Format::RGB32_sfloat:      return VK_FORMAT_R32G32B32_SFLOAT;
		case Format::RGBA32_sfloat:     return VK_FORMAT_R32G32B32A32_SFLOAT;

		case Format::D16_unorm:         return VK_FORMAT_D16_UNORM;
		case Format::D32_sfloat:        return VK_FORMAT_D32_SFLOAT;
		case Format::D24_unorm_S8_uint: return VK_FORMAT_D24_UNORM_S8_UINT;
		default: return VK_FORMAT_UNDEFINED;
	}
	return VK_FORMAT_UNDEFINED;
}

inline constexpr VkFilter VkFilter(enum Filter value) {
	switch (value) {
		case Filter::Nearest:   return VK_FILTER_NEAREST;
		case Filter::Linear:    return VK_FILTER_LINEAR;
		case Filter::Cubic_Ext: return VK_FILTER_CUBIC_EXT;
		default: return VK_FILTER_LINEAR;
	}
	return VK_FILTER_LINEAR;
}

inline constexpr VkSamplerMipmapMode VkSamplerMipmapMode(enum MipmapMode value) {
	switch (value) {
		case MipmapMode::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case MipmapMode::Linear:  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		default: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}
	return VK_SAMPLER_MIPMAP_MODE_LINEAR;
}

inline constexpr VkSamplerAddressMode VkSamplerAddressMode(enum WrapMode value) {
	switch (value) {
		case WrapMode::Repeat:            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case WrapMode::MirroredRepeat:    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case WrapMode::ClampToEdge:       return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case WrapMode::ClampToBorder:     return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case WrapMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}

} // namespace Cast


struct Context {	
	// void EndCommandBuffer(VkSubmitInfo submitInfo);
	// Command::void EndCommandBuffer(VkSubmitInfo submitInfo);
	VkAllocationCallbacks* allocator = VK_NULL_HANDLE;


	// void LoadShaders(Pipeline& pipeline);
	// std::vector<char> CompileShader(const std::filesystem::path& path, const char* entryPoint);

	void CreatePipelineLibrary(const PipelineDesc& desc, Pipeline& pipeline);

	bool presentRequested = false;

#ifdef NRAY_DEBUG
	static constexpr bool enableValidationLayers = true;
#else
	static constexpr bool enableValidationLayers = false;
#endif
	static constexpr bool enableDebugReport = false;
	
	static constexpr bool usePipelineLibrary = false;
	static constexpr bool linkTimeOptimization = true; // Pipeline library link

	static constexpr bool enableUnusedAttachments = false;


	bool imguiInitialized = false;

	std::vector<const char*> requiredInstanceExtensions {
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
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
	std::string applicationName = "nRay";
	std::string engineName = "nRay Engine";

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
	SampleCount maxSamples = SampleCount::_1;
	SampleCount numSamples = SampleCount::_1;
	VkSampleCountFlags sampleCounts;

	bool descriptorIndexingAvailable = false;
	
	VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT unusedAttachmentFeatures{};
	VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT graphicsPipelineLibraryFeatures{};
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{};
	VkPhysicalDeviceSynchronization2FeaturesKHR synchronization2Features{};
	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
	VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
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
	uint32_t GetQueueFamilyIndex(VkQueueFlags desiredFlags, VkQueueFlags undesiredFlags, const std::span<const std::pair<VkQueueFlags, float>>& avoidIfPossible = {}, VkSurfaceKHR surfaceToSupport = VK_NULL_HANDLE, const std::span<const uint32_t>& filledUpFamilies = {});
	std::vector<std::string_view> GetSupportedExtensions(const std::span<const char*>& desiredExtensions);
};

struct QueueResource {
	uint32_t familyIndex = ~0;
	uint32_t index = 0;
	QueueFlags flags;
	VkQueue handle = VK_NULL_HANDLE;
	Command command {};
	// UID uid;
};



struct DeviceResource: DeleteCopyMove {
	UID uid;
	DeviceResource(UID uid) : uid(uid) {
		pipelineLibrary.device = this;
	}

	Pipeline CreatePipeline(const PipelineDesc& desc);
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
	std::vector<int32_t> availableStorageImageRID;
	VkSampler genericSampler;

	VkPipelineCache pipelineCache = VK_NULL_HANDLE;
	
	struct PipelineLibrary {
		struct VertexInputInfo {
			std::vector<Format> vertexAttributes;
			bool lineTopology = false;
		};

		struct PreRasterizationInfo {
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			bool lineTopology = false;
			CullModeFlags cullMode = CullMode::None;
			std::vector<Pipeline::Stage> stages;

		};

		struct FragmentShaderInfo {
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			SampleCount samples;
			std::vector<Pipeline::Stage> stages;
		};

		struct FragmentOutputInfo {
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			std::vector<Format> colorFormats;
			bool useDepth;
			Format depthFormat;
			SampleCount samples;
		};
		
		struct VertexInputHash {
			size_t operator()(const VertexInputInfo& info) const {
				return HashCombine(VectorHash(info.vertexAttributes), info.lineTopology);
			}

			bool operator()(const VertexInputInfo& a, const VertexInputInfo& b) const {
				return a.vertexAttributes == b.vertexAttributes && a.lineTopology == b.lineTopology;
			}
		};

		struct PreRasterizationHash {
			size_t operator()(const PreRasterizationInfo& info) const {
				size_t seed = 0;
				for (const Pipeline::Stage &stage: info.stages) {
					seed = HashCombine(seed, stage.Hash());
				}
				seed = HashCombine(seed, info.lineTopology);
				seed = HashCombine(seed, info.cullMode);
				return seed;
			}
			
			bool operator()(const PreRasterizationInfo& a, const PreRasterizationInfo& b) const {
				return a.stages.size() == b.stages.size()
					&& std::equal(a.stages.begin(), a.stages.end(), b.stages.begin(), [](const auto& a, const auto& b) {
						return a.stage == b.stage && a.path == b.path && a.entryPoint == b.entryPoint;
					})
					&& a.lineTopology == b.lineTopology && a.cullMode == b.cullMode;
			}
		};

		struct FragmentOutputHash {
			size_t operator()(const FragmentOutputInfo& info) const {
				size_t hash = HashCombine(VectorHash(info.colorFormats), reinterpret_cast<size_t>(info.pipelineLayout));
				hash = HashCombine(hash, info.useDepth);
				hash = HashCombine(hash, static_cast<size_t>(info.depthFormat));
				hash = HashCombine(hash, static_cast<size_t>(info.samples));
				return hash;
			}

			bool operator()(const FragmentOutputInfo& a, const FragmentOutputInfo& b) const {
				return std::equal(a.colorFormats.begin(), a.colorFormats.end(), b.colorFormats.begin())
					&& a.pipelineLayout == b.pipelineLayout
					&& a.useDepth == b.useDepth
					&& a.depthFormat == b.depthFormat
					&& a.samples == b.samples;
			}
		};

		struct FragmentShaderHash {
			size_t operator()(const FragmentShaderInfo& info) const {
				size_t hash = 0;
				for (const auto& stage: info.stages) {
					hash = HashCombine(hash, stage.Hash());
				}
				hash = HashCombine(hash, reinterpret_cast<size_t>(info.pipelineLayout));
				hash = HashCombine(hash, static_cast<size_t>(info.samples));
				return hash;
			}
			bool operator()(const FragmentShaderInfo& a, const FragmentShaderInfo& b) const {
				return a.stages.size() == b.stages.size()
					&& std::equal(a.stages.begin(), a.stages.end(), b.stages.begin(), [](const auto& a, const auto& b) {
						return a.stage == b.stage && a.path == b.path && a.entryPoint == b.entryPoint;
					})
					&& a.pipelineLayout == b.pipelineLayout
					&& a.samples == b.samples;
			}
		};

		DeviceResource* device = nullptr;
		std::unordered_map<VertexInputInfo, VkPipeline, VertexInputHash, VertexInputHash> vertexInputInterfaces;
		std::unordered_map<PreRasterizationInfo, VkPipeline, PreRasterizationHash, PreRasterizationHash> preRasterizationShaders;
		std::unordered_map<FragmentOutputInfo, VkPipeline, FragmentOutputHash, FragmentOutputHash> fragmentOutputInterfaces;
		std::unordered_map<FragmentShaderInfo, VkPipeline, FragmentShaderHash, FragmentShaderHash> fragmentShaders;

		void CreateVertexInputInterface(VertexInputInfo info);
		void CreatePreRasterizationShaders(PreRasterizationInfo info);
		void CreateFragmentShader(FragmentShaderInfo info);
		void CreateFragmentOutputInterface(FragmentOutputInfo info);
		VkPipeline LinkPipeline(const std::array<VkPipeline, 4> pipelines, VkPipelineLayout layout);

	} pipelineLibrary;
	
	const uint32_t stagingBufferSize = 32 * 1024 * 1024;
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

	VkSampler CreateSampler(VkDevice device, SamplerDesc desc);

	void SetDebugUtilsObjectName(const VkDebugUtilsObjectNameInfoEXT& pNameInfo);
	
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
		if (imguiDescriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(handle, imguiDescriptorPool, _ctx.allocator);
		}
		
		for (auto& [_, pipeline]: pipelineLibrary.vertexInputInterfaces) {
			vkDestroyPipeline(handle, pipeline, _ctx.allocator);
		}
		for (auto& [_, pipeline]: pipelineLibrary.preRasterizationShaders) {
			vkDestroyPipeline(handle, pipeline, _ctx.allocator);
		}
		for (auto& [_, pipeline]: pipelineLibrary.fragmentOutputInterfaces) {
			vkDestroyPipeline(handle, pipeline, _ctx.allocator);
		}
		for (auto& [_, pipeline]: pipelineLibrary.fragmentShaders) {
			vkDestroyPipeline(handle, pipeline, _ctx.allocator);
		}

		staging = {};
		stagingCpu = nullptr;
		DestroyBindlessDescriptorResources();
		vmaDestroyAllocator(vmaAllocator);
		vkDestroySampler(handle, genericSampler, _ctx.allocator);

		for (auto& [_, queue] : uniqueQueues) {
			queue->command.resource.reset();
		}
		vkDestroyDevice(handle, _ctx.allocator);
		DEBUG_TRACE("Destroyed logical device");
		handle = VK_NULL_HANDLE;
	}
};

struct CommandResource {
	DeviceResource* device = nullptr;
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
		vkDestroyCommandPool(device->handle, pool, _ctx.allocator);
		vkDestroyFence(device->handle, fence, _ctx.allocator);
		// vkDestroyQueryPool(device, queryPool, _ctx.allocator);
	}
};


struct SwapChainResource: DeleteCopyMove {
	DeviceResource* device = nullptr;
	// VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;	
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> imageResources;
	std::vector<VkImageView> imageViews;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkPresentModeKHR> availablePresentModes;
	std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;

	std::vector<Command> commands; // Owns all command resources

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

	void CreateImGui(GLFWwindow* window, SampleCount sampleCount);
	void DestroyImGui();

	inline VkSemaphore GetImageAvailableSemaphore(uint32_t currentFrame) { return imageAvailableSemaphores[currentFrame]; }
	inline VkSemaphore GetRenderFinishedSemaphore(uint32_t currentFrame) { return renderFinishedSemaphores[currentFrame]; }
	bool SupportFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);
	void ChooseExtent(uint32_t width, uint32_t height);
	void ChoosePresentMode();
	void ChooseSurfaceFormat();
};


struct Resource {
	DeviceResource* device;
	std::string name;
	int32_t rid = Null;
	static inline constexpr int32_t Null = ~0;
	Resource(DeviceResource* device, const std::string& name = "") : device(device), name(name) {}
	virtual ~Resource() = default;
};

struct BufferResource : Resource {
	VkBuffer buffer;
	VmaAllocation allocation;

	using Resource::Resource;

	~BufferResource() override {
		vmaDestroyBuffer(device->vmaAllocator, buffer, allocation);
		if (rid != Null) {
			device->availableBufferRID.push_back(rid);
		}
	}
};

struct ImageResource : Resource {
	VkImage image;
	VkImageView view;
	VmaAllocation allocation;
	bool fromSwapchain = false;
	SampleCount samples = SampleCount::_1;
	std::vector<VkImageView> layersView;
	std::vector<ImTextureID> imguiRIDs;

	using Resource::Resource;
	ImageResource(DeviceResource* device, VkImage image, VkImageView view, bool fromSwapchain, const std::string& name = ""):
		Resource(device, name),
		image(image),
		view(view),
		fromSwapchain(fromSwapchain) {}

	virtual ~ImageResource() {
		if (!fromSwapchain) {
			// printf("destroy image %s\n", name.c_str());
			for (VkImageView layerView : layersView) {
				vkDestroyImageView(device->handle, layerView, _ctx.allocator);
			}
			layersView.clear();
			vkDestroyImageView(device->handle, view, _ctx.allocator);
			vmaDestroyImage(device->vmaAllocator, image, allocation);
			if (rid != Null) {
				device->availableImageRID.push_back(rid);
				for (ImTextureID imguiRID : imguiRIDs) {
				    ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)imguiRID);
				}
				rid = Null;
				imguiRIDs.clear();
			}
		}
	}
};

struct PipelineResource : Resource {
	VkPipeline pipeline;
	VkPipelineLayout layout;

	using Resource::Resource;

	void CreatePipelineLayout(const std::span<const VkDescriptorSetLayout>& descriptorLayouts);

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
	resource.staging = device.CreateBuffer({resource.stagingBufferSize, BufferUsage::TransferSrc, Memory::CPU, "StagingBuffer[" + std::to_string(uid) + "]"});
	resource.stagingCpu = (u8*)resource.staging.resource->allocation->GetMappedData();
	return device;
}


void Destroy() {
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

void Device::ResetStaging(){
	resource->stagingOffset = 0;
}

Command& Queue::GetCommandBuffer() {
	return resource->command; 
}

Queue::Queue(const std::shared_ptr<QueueResource>& resource):
	resource(resource),
	flags(resource->flags) {}

Queue Device::GetQueue(QueueFlags flags){
	for (auto& [_, queue] : resource->uniqueQueues) {
		if (queue->flags & flags) {
			return Queue(queue);
		}
	}
	return Queue();
}


Buffer Device::CreateBuffer(const BufferDesc& desc) {
	BufferUsageFlags usage = desc.usage;
	uint32_t size = desc.size;

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

	std::shared_ptr<BufferResource> res = std::make_shared<BufferResource>(this->resource.get(), desc.name);

	VkBufferCreateInfo bufferInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = static_cast<VkBufferUsageFlags>(usage),
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};

	constexpr VmaAllocationCreateFlags cpuFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	
	VmaAllocationCreateInfo allocInfo = {
		.flags = desc.memory & Memory::CPU? cpuFlags: 0,
		.usage = VMA_MEMORY_USAGE_AUTO,
	};

	auto result = vmaCreateBuffer(resource->vmaAllocator, &bufferInfo, &allocInfo, &res->buffer, &res->allocation, nullptr);
	DEBUG_VK(result, "Failed to create buffer!");
	ASSERT(result == VK_SUCCESS, "Failed to create buffer!");
	// LOG_INFO("Created buffer {}", name);

	Buffer buffer = {
		.resource = res,
		.size = size,
		.usage = usage,
		.memory = desc.memory,
	};

	if (usage & BufferUsage::Storage) {
		res->rid = resource->availableBufferRID.back();
		resource->availableBufferRID.pop_back();

		VkDescriptorBufferInfo bufferInfo = {
			.buffer = res->buffer,
			.offset = 0,
			.range  = size,
		};

		VkWriteDescriptorSet write = {
			.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet          = resource->bindlessDescriptorSet,
			.dstBinding      = BINDING_BUFFER,
			.dstArrayElement = buffer.RID(),
			.descriptorCount = 1,
			.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.pBufferInfo     = &bufferInfo,
		};

		vkUpdateDescriptorSets(resource->handle, 1, &write, 0, nullptr);
	}

	return buffer;
}

Image Device::CreateImage(const ImageDesc& desc) {
	std::shared_ptr<ImageResource> res = std::make_shared<ImageResource>(this->resource.get(), desc.name);
	res->samples = desc.samples;
	VkImageCreateInfo imageInfo{
		.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.flags         = (VkImageCreateFlags)(desc.layers == 6? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT: 0),
		.imageType     = VK_IMAGE_TYPE_2D,
		.format        = Cast::VkFormat(desc.format),
		.extent = {	
			.width     = desc.width,
			.height    = desc.height,
			.depth     = 1,
		},
		.mipLevels     = 1,
		.arrayLayers   = desc.layers,
		.samples       = (VkSampleCountFlagBits)std::min(desc.samples, resource->physicalDevice->maxSamples),
		.tiling        = VK_IMAGE_TILING_OPTIMAL,
		.usage         = (VkImageUsageFlags)desc.usage,
		.sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};


	VmaAllocationCreateInfo allocInfo = {
		.usage = VMA_MEMORY_USAGE_AUTO,
	 	.preferredFlags = (VkMemoryPropertyFlags)(desc.usage & ImageUsage::TransientAttachment? VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT: 0),
	};
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

	VkImageViewCreateInfo viewInfo{
		.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image    = res->image,
		.viewType = desc.layers == 1? VK_IMAGE_VIEW_TYPE_2D: VK_IMAGE_VIEW_TYPE_CUBE,
		.format   = (VkFormat)desc.format,
		.subresourceRange {
			.aspectMask     = (VkImageAspectFlags)aspect,
			.baseMipLevel   = 0,
			.levelCount     = 1,
			.baseArrayLayer = 0,
			.layerCount     = desc.layers
		}
	};

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
		switch (aspect) {
		case Aspect::Depth:                   newLayout = ImageLayout::DepthReadOnly;    break;
		case Aspect::Depth | Aspect::Stencil: newLayout = ImageLayout::DepthStencilRead; break;
		default: break;
		}
		res->imguiRIDs.resize(desc.layers);
		
		for (int i = 0; i < desc.layers; i++) {
			res->imguiRIDs[i] = (ImTextureID) ImGui_ImplVulkan_AddTexture(resource->genericSampler, res->layersView[i], (VkImageLayout)newLayout);
		}

		VkDescriptorImageInfo descriptorInfo = {
			.sampler = resource->genericSampler,
			.imageView = res->view,
			.imageLayout = (VkImageLayout)newLayout,
		};
		VkWriteDescriptorSet write = {
			.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet          = resource->bindlessDescriptorSet,
			.dstBinding      = BINDING_TEXTURE,
			.dstArrayElement = image.RID(),
			.descriptorCount = 1,
			.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo      = &descriptorInfo,
		};

		vkUpdateDescriptorSets(resource->handle, 1, &write, 0, nullptr);
	}
	if (desc.usage & ImageUsage::Storage) {
		VkDescriptorImageInfo descriptorInfo = {
			.sampler     = resource->genericSampler,
			.imageView   = res->view,
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
		};
		VkWriteDescriptorSet write = {
			.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet          = resource->bindlessDescriptorSet,
			.dstBinding      = BINDING_STORAGE_IMAGE,
			.dstArrayElement = image.RID(),
			.descriptorCount = 1,
			.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			.pImageInfo      = &descriptorInfo,
		};
		
		vkUpdateDescriptorSets(resource->handle, 1, &write, 0, nullptr);
	}

	resource->SetDebugUtilsObjectName({
		.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType   = VkObjectType::VK_OBJECT_TYPE_IMAGE,
		.objectHandle = (uint64_t)(VkImage)res->image,
		.pObjectName  = desc.name.c_str(),
	});

	resource->SetDebugUtilsObjectName({
		.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType   = VkObjectType::VK_OBJECT_TYPE_IMAGE_VIEW,
		.objectHandle = (uint64_t)(VkImageView)res->view,
		.pObjectName  = (desc.name + "View").c_str(),
	});
	return image;
}


void Command::Dispatch(const uvec3& groups) {
	vkCmdDispatch(resource->buffer, groups.x, groups.y, groups.z);
}

std::vector<char> ReadBinaryFile(const std::string& path) {
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	// LOG_DEBUG("Reading binary file: " + path);
	ASSERT(file.is_open(), "Failed to open file! {}", path);
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}


std::vector<char> CompileShader(const std::filesystem::path& path, const char* entryPoint) {
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

std::vector<char> LoadShader(Pipeline::Stage stage) {
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
		return std::move(CompileShader(stage.path, stage.entryPoint.c_str()));
	} else {
		return std::move(ReadBinaryFile(binPath.string()));
	}
}

void PipelineResource::CreatePipelineLayout(const std::span<const VkDescriptorSetLayout>& descriptorLayouts) {

    VkPushConstantRange pushConstant{
		.stageFlags = VK_SHADER_STAGE_ALL,
		.offset     = 0,
		.size       = device->physicalDevice->physicalProperties2.properties.limits.maxPushConstantsSize,
	};
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount         = (uint32_t)(descriptorLayouts.size()),
		.pSetLayouts            = descriptorLayouts.data(),
		.pushConstantRangeCount = 1,
		.pPushConstantRanges    = &pushConstant,
	};

    auto vkRes = vkCreatePipelineLayout(device->handle, &pipelineLayoutInfo, _ctx.allocator, &this->layout);
    DEBUG_VK(vkRes, "Failed to create pipeline layout!");
    ASSERT(vkRes == VK_SUCCESS, "Failed to create pipeline layout!");
}

Pipeline Device::CreatePipeline(const PipelineDesc& desc){
	return resource->CreatePipeline(desc);
}

Pipeline DeviceResource::CreatePipeline(const PipelineDesc& desc) { // External handle
	Pipeline pipeline;
	pipeline.point = desc.point;
	pipeline.resource = std::make_shared<PipelineResource>(this, desc.name);
	pipeline.resource->CreatePipelineLayout({{bindlessDescriptorLayout}});

	if (_ctx.usePipelineLibrary && desc.point == PipelinePoint::Graphics && physicalDevice->graphicsPipelineLibraryFeatures.graphicsPipelineLibrary) {
	// if (0) {
		// VertexInputInterface
		DeviceResource::PipelineLibrary::VertexInputInfo vertexInputInfo { desc.vertexAttributes, desc.lineTopology };
		if (!pipelineLibrary.vertexInputInterfaces.count(vertexInputInfo)) {
			pipelineLibrary.CreateVertexInputInterface(vertexInputInfo);
		}

		PipelineLibrary::PreRasterizationInfo preRasterizationInfo { pipeline.resource->layout, desc.lineTopology, desc.cullMode };
		PipelineLibrary::FragmentShaderInfo   fragmentShaderInfo   { pipeline.resource->layout, desc.samples};
		for (auto& stage: desc.stages) {
			switch(stage.stage) {
				case ShaderStage::Vertex:
				case ShaderStage::TessellationControl:
				case ShaderStage::TessellationEvaluation:
				case ShaderStage::Geometry:
				case ShaderStage::Task:
				case ShaderStage::Mesh:
				preRasterizationInfo.stages.push_back(stage);
				break;
				case ShaderStage::Fragment:
				fragmentShaderInfo.stages.push_back(stage); break;
				default:
				break;
			}
		}

		if (!pipelineLibrary.preRasterizationShaders.count(preRasterizationInfo)) {
			pipelineLibrary.CreatePreRasterizationShaders(preRasterizationInfo);
		}
		if (!pipelineLibrary.fragmentShaders.count(fragmentShaderInfo)) {
			pipelineLibrary.CreateFragmentShader(fragmentShaderInfo);
		}

		PipelineLibrary::FragmentOutputInfo fragmentOutputInfo { pipeline.resource->layout, desc.colorFormats, desc.useDepth, desc.depthFormat, desc.samples};
		if (!pipelineLibrary.fragmentOutputInterfaces.count(fragmentOutputInfo)) {
			pipelineLibrary.CreateFragmentOutputInterface(fragmentOutputInfo);
		}

		pipeline.resource->pipeline = pipelineLibrary.LinkPipeline({
			pipelineLibrary.vertexInputInterfaces[vertexInputInfo],
			pipelineLibrary.preRasterizationShaders[preRasterizationInfo],
			pipelineLibrary.fragmentShaders[fragmentShaderInfo],
			pipelineLibrary.fragmentOutputInterfaces[fragmentOutputInfo]
		},
		pipeline.resource->layout);

	} else {
		CreatePipelineImpl(desc, pipeline);
	}
	return pipeline;
}

void DeviceResource::CreatePipelineImpl(const PipelineDesc& desc, Pipeline& pipeline) {
	pipeline.point = desc.point; // Graphics or Compute

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.reserve(desc.stages.size());
	std::vector<VkShaderModule> shaderModules;
	shaderModules.reserve(desc.stages.size());
	for (auto& stage: desc.stages) {
		std::vector<char> bytes = LoadShader(stage);
		VkShaderModuleCreateInfo createInfo{};
		VkShaderModule shaderModule;
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = bytes.size();
		createInfo.pCode = (const uint32_t*)(bytes.data());
		auto result = vkCreateShaderModule(handle, &createInfo, _ctx.allocator, &shaderModule);
		DEBUG_VK(result, "Failed to create shader module!");
		ASSERT(result == VK_SUCCESS, "Failed to create shader module!");
		shaderModules.push_back(shaderModule);
		VkPipelineShaderStageCreateInfo shaderStageInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = (VkShaderStageFlagBits)stage.stage,
			.module = shaderModule,
			.pName = stage.entryPoint.c_str(),
			.pSpecializationInfo = nullptr,
		};
		shaderStages.push_back(shaderStageInfo);
	}

	// std::vector<VkDescriptorSetLayout> layouts;
	// layouts.push_back(bindlessDescriptorLayout);
	// // layouts.push_back(descriptorSetLayout);

	// // TODO: opt, specify each time, but keep compatible(same) for all pipeline layouts
	// VkPushConstantRange pushConstant{};
	// pushConstant.offset = 0;
	// pushConstant.size = physicalDevice->physicalProperties2.properties.limits.maxPushConstantsSize;
	// pushConstant.stageFlags = VK_SHADER_STAGE_ALL;

	// VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	// pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// pipelineLayoutInfo.setLayoutCount = layouts.size();
	// pipelineLayoutInfo.pSetLayouts = layouts.data();
	// pipelineLayoutInfo.pushConstantRangeCount = 1;
	// pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

	// auto vkRes = vkCreatePipelineLayout(handle, &pipelineLayoutInfo, _ctx.allocator, &pipeline.resource->layout);
	// DEBUG_VK(vkRes, "Failed to create pipeline layout!");
	// ASSERT(vkRes == VK_SUCCESS, "Failed to create pipeline layout!");

	if (desc.point == PipelinePoint::Compute) {
		ASSERT(shaderStages.size() == 1, "Compute pipeline only support 1 stage.");

		VkComputePipelineCreateInfo pipelineInfo {
			.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.stage = shaderStages[0],
			.layout = pipeline.resource->layout,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1,
		};

		auto vkRes = vkCreateComputePipelines(handle, VK_NULL_HANDLE, 1, &pipelineInfo, _ctx.allocator, &pipeline.resource->pipeline);

		DEBUG_VK(vkRes, "Failed to create compute pipeline!");
		ASSERT(vkRes == VK_SUCCESS, "Failed to create compute pipeline!");
	} else {
		// graphics pipeline
		VkPipelineRasterizationStateCreateInfo rasterizationState = {
			.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable        = VK_FALSE, // fragments beyond near and far planes are clamped to them
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode             = VK_POLYGON_MODE_FILL,
			.cullMode                = (VkCullModeFlags)desc.cullMode,
			.frontFace               = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable         = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp          = 0.0f,
			.depthBiasSlopeFactor    = 0.0f,
			.lineWidth               = 1.0f, // line thickness in terms of number of fragments
		};
		

		VkPipelineMultisampleStateCreateInfo multisampleState = {
			.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples  = (VkSampleCountFlagBits)std::min(desc.samples, physicalDevice->maxSamples),
			.sampleShadingEnable   = VK_FALSE,
			.minSampleShading      = 0.5f,
			.pSampleMask           = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable      = VK_FALSE,
		};

		VkPipelineDepthStencilStateCreateInfo depthStencilState = {
			.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable       = VK_TRUE,
			.depthWriteEnable      = VK_TRUE,
			.depthCompareOp        = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable     = VK_FALSE,
			.front                 = {},
			.back                  = {},
			.minDepthBounds        = 0.0f,
			.maxDepthBounds        = 1.0f,
		};

		std::vector<VkVertexInputAttributeDescription> attributeDescs;
		attributeDescs.reserve(desc.vertexAttributes.size());
		uint32_t attributeSize = 0;
		for (size_t i = 0; auto& format: desc.vertexAttributes) {
			attributeDescs.emplace_back(VkVertexInputAttributeDescription{
				.location = static_cast<uint32_t>(i++),
				.binding = 0,
				.format = (VkFormat)format,
				.offset = attributeSize
			});
			switch (format) {
			case Format::RG32_sfloat:   attributeSize += 2 * sizeof(float); break;
			case Format::RGB32_sfloat:  attributeSize += 3 * sizeof(float); break;
			case Format::RGBA32_sfloat: attributeSize += 4 * sizeof(float); break;
			default: DEBUG_ASSERT(false, "Invalid Vertex Attribute"); break;
			}
		}

		VkVertexInputBindingDescription bindingDescription{
			.binding   = 0,
			.stride    = attributeSize,
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
		};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &bindingDescription,
			.vertexAttributeDescriptionCount = (uint32_t)(attributeDescs.size()),
			.pVertexAttributeDescriptions = attributeDescs.data(),
		};

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		
		// define the type of input of our pipeline
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = desc.lineTopology? VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			// with this parameter true we can break up lines and triangles in _STRIP topology modes
			.primitiveRestartEnable = VK_FALSE,
		};

		VkPipelineDynamicStateCreateInfo dynamicInfo{
			.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates    = dynamicStates.data(),
		};

		VkPipelineViewportStateCreateInfo viewportState{
			.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports    = nullptr,
			.scissorCount  = 1,
			.pScissors     = nullptr,
		};

		VkPipelineRenderingCreateInfoKHR pipelineRendering{
			.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
			.viewMask                = 0,
			.colorAttachmentCount    = static_cast<uint32_t>(desc.colorFormats.size()),
			.pColorAttachmentFormats = reinterpret_cast<const VkFormat*>(desc.colorFormats.data()),
			.depthAttachmentFormat   = desc.useDepth ? static_cast<VkFormat>(desc.depthFormat) : VK_FORMAT_UNDEFINED,
			.stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
		};

		std::vector<VkPipelineColorBlendAttachmentState> blendAttachments(desc.colorFormats.size(), {
			.blendEnable         = VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
			.colorBlendOp        = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		});

		VkPipelineColorBlendStateCreateInfo colorBlendState {
			.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable   = VK_FALSE,
			.logicOp         = VK_LOGIC_OP_COPY,
			.attachmentCount = static_cast<uint32_t>(blendAttachments.size()),
			.pAttachments    = blendAttachments.data(),
			.blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f},
		};

		VkGraphicsPipelineCreateInfo pipelineInfo{
			.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext               = &pipelineRendering,
			.stageCount          = static_cast<uint32_t>(shaderStages.size()),
			.pStages             = shaderStages.data(),
			.pVertexInputState   = &vertexInputInfo,
			.pInputAssemblyState = &inputAssembly,
			.pViewportState      = &viewportState,
			.pRasterizationState = &rasterizationState,
			.pMultisampleState   = &multisampleState,
			.pDepthStencilState  = &depthStencilState,
			.pColorBlendState    = &colorBlendState,
			.pDynamicState       = &dynamicInfo,
			.layout              = pipeline.resource->layout,
			.basePipelineHandle  = VK_NULL_HANDLE,
			.basePipelineIndex   = -1,
		};

		auto vkRes = vkCreateGraphicsPipelines(handle, VK_NULL_HANDLE, 1, &pipelineInfo, _ctx.allocator, &pipeline.resource->pipeline);
		DEBUG_VK(vkRes, "Failed to create graphics pipeline!");
	}

	for (int i = 0; i < shaderModules.size(); i++) {
		vkDestroyShaderModule(handle, shaderModules[i], _ctx.allocator);
	}
}

void DeviceResource::PipelineLibrary::CreateVertexInputInterface(VertexInputInfo info) {
	VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
		.flags = VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT,
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = info.lineTopology? VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		// with this parameter true we can break up lines and triangles in _STRIP topology modes
		.primitiveRestartEnable = VK_FALSE,
	};

	std::vector<VkVertexInputAttributeDescription> attributeDescs;
	attributeDescs.reserve(info.vertexAttributes.size());
	uint32_t attributeSize = 0;
	for (size_t i = 0; auto& format: info.vertexAttributes) {
		attributeDescs.emplace_back(VkVertexInputAttributeDescription{
			.location = static_cast<uint32_t>(i++),
			.binding = 0,
			.format = (VkFormat)format,
			.offset = attributeSize
		});
		switch (format) {
		case Format::RG32_sfloat:   attributeSize += 2 * sizeof(float); break;
		case Format::RGB32_sfloat:  attributeSize += 3 * sizeof(float); break;
		case Format::RGBA32_sfloat: attributeSize += 4 * sizeof(float); break;
		default: DEBUG_ASSERT(false, "Invalid Vertex Attribute"); break;
		}
	}

	VkVertexInputBindingDescription bindingDescription{
		.binding = 0,
		.stride = attributeSize,
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
	};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &bindingDescription,
		.vertexAttributeDescriptionCount = (uint32_t)(attributeDescs.size()),
		.pVertexAttributeDescriptions = attributeDescs.data(),
	};

	VkGraphicsPipelineCreateInfo pipelineLibraryInfo {
		.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext               = &libraryInfo,
		.flags               = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
		.pVertexInputState   = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pDynamicState       = nullptr
	};
	
	VkPipeline pipeline;
	auto res = vkCreateGraphicsPipelines(device->handle, device->pipelineCache, 1, &pipelineLibraryInfo, _ctx.allocator, &pipeline);
	DEBUG_VK(res, "Failed to create vertex input interface!");
	vertexInputInterfaces.emplace(info, pipeline);
}

void DeviceResource::PipelineLibrary::CreatePreRasterizationShaders(PreRasterizationInfo info) {
	VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
		.flags = VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT,
	};

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	if (info.lineTopology) {
		dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
	}

	VkPipelineDynamicStateCreateInfo dynamicInfo{
		.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates    = dynamicStates.data(),
	};

	VkPipelineViewportStateCreateInfo viewportState{
		.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports    = nullptr,
		.scissorCount  = 1,
		.pScissors     = nullptr,
	};

	VkPipelineRasterizationStateCreateInfo rasterizationState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE, // fragments beyond near and far planes are clamped to them
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = (VkCullModeFlags)info.cullMode, // line thickness in terms of number of fragments
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f,
	};

	// Using the pipeline library extension, we can skip the pipeline shader module creation and directly pass the shader code to the pipeline
	// std::vector<VkShaderModuleCreateInfo> shaderModuleInfos;
	// shaderModuleInfos.reserve(info.stages.size());
	// std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	// shaderStages.reserve(info.stages.size());
	// std::vector<std::vector<char>> bytes;
	// bytes.reserve(info.stages.size());
	// for (auto& stage: info.stages) {
	// 	bytes.emplace_back(LoadShader(stage));

	// 	shaderModuleInfos.emplace_back(VkShaderModuleCreateInfo{
	// 		.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
	// 		.codeSize =  bytes.size(),
	// 		.pCode    = (const uint32_t*)bytes.data()
	// 	});

	// 	shaderStages.emplace_back(VkPipelineShaderStageCreateInfo {
	// 		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	// 		.pNext = &shaderModuleInfos.back(),
	// 		.stage = (VkShaderStageFlagBits)stage.stage,
	// 		.pName = stage.entryPoint.c_str(),
	// 		.pSpecializationInfo = nullptr,
	// 	});
	// }

	std::vector<VkShaderModuleCreateInfo> shaderModuleInfos(info.stages.size());
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(info.stages.size());
	std::vector<std::vector<char>> bytes(info.stages.size());
	for (size_t i = 0; i < info.stages.size(); ++i) {
		auto& stage = info.stages[i];

		bytes[i] = std::move(LoadShader(stage));

		shaderModuleInfos[i] = VkShaderModuleCreateInfo{
			.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = bytes[i].size(),
			.pCode    = (const uint32_t*)bytes[i].data()
		};

		shaderStages[i] = VkPipelineShaderStageCreateInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = &shaderModuleInfos[i],
			.stage = (VkShaderStageFlagBits)stage.stage,
			.pName = stage.entryPoint.c_str(),
			.pSpecializationInfo = nullptr,
		};
	}

	VkGraphicsPipelineCreateInfo pipelineLibraryInfo {
		.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext               = &libraryInfo,
		.flags               = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
		.stageCount          = static_cast<uint32_t>(shaderStages.size()),
		.pStages             = shaderStages.data(),
		.pViewportState      = &viewportState,
		.pRasterizationState = &rasterizationState,
		.pDynamicState       = &dynamicInfo,
		.layout              = info.pipelineLayout,
	};
	VkPipeline pipeline;
	auto res = vkCreateGraphicsPipelines(device->handle, device->pipelineCache, 1, &pipelineLibraryInfo , _ctx.allocator, &pipeline);
	DEBUG_VK(res, "Failed to create pre-rasterization shaders!");
	preRasterizationShaders.emplace(info, pipeline);
}

void DeviceResource::PipelineLibrary::CreateFragmentShader(FragmentShaderInfo info) {
	VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
		.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT,
	};

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
		.rasterizationSamples = (VkSampleCountFlagBits)std::min(info.samples, device->physicalDevice->maxSamples),
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 0.5f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
	};
	

	std::vector<VkShaderModuleCreateInfo> shaderModuleInfos(info.stages.size());
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(info.stages.size());
	std::vector<std::vector<char>> bytes(info.stages.size());
	for (size_t i = 0; i < info.stages.size(); ++i) {
		auto& stage = info.stages[i];

		bytes[i] = std::move(LoadShader(stage));

		shaderModuleInfos[i] = VkShaderModuleCreateInfo{
			.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = bytes[i].size(),
			.pCode    = (const uint32_t*)bytes[i].data()
		};

		shaderStages[i] = VkPipelineShaderStageCreateInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = &shaderModuleInfos[i],
			.stage = (VkShaderStageFlagBits)stage.stage,
			.pName = stage.entryPoint.c_str(),
			.pSpecializationInfo = nullptr,
		};
	}

	VkGraphicsPipelineCreateInfo pipelineLibraryInfo {
		.sType              = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext              = &libraryInfo,
		.flags              = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
		.stageCount          = static_cast<uint32_t>(shaderStages.size()),
		.pStages             = shaderStages.data(),
		.pMultisampleState  = &multisampleState,
		.pDepthStencilState = &depthStencilState,
		.layout             = info.pipelineLayout,
	};

	VkPipeline pipeline;
	auto res = vkCreateGraphicsPipelines(device->handle, device->pipelineCache, 1, &pipelineLibraryInfo, nullptr, &pipeline); //todo: Thread pipeline cache
	DEBUG_VK(res, "Failed to create vertex input interface!");
	fragmentShaders.emplace(info, pipeline);
}

void DeviceResource::PipelineLibrary::CreateFragmentOutputInterface(FragmentOutputInfo info) {
	VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
		.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT,
	};

	std::vector<VkPipelineColorBlendAttachmentState> blendAttachments(info.colorFormats.size(), {
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	});

	VkPipelineColorBlendStateCreateInfo colorBlendState {
		.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable   = VK_FALSE,
		.logicOp         = VK_LOGIC_OP_COPY,
		.attachmentCount = static_cast<uint32_t>(blendAttachments.size()),
		.pAttachments    = blendAttachments.data(),
		.blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f},
	};

	VkPipelineMultisampleStateCreateInfo multisampleState {
		.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples  = (VkSampleCountFlagBits)std::min(info.samples, device->physicalDevice->maxSamples),
		.sampleShadingEnable   = VK_FALSE,
		.minSampleShading      = 0.5f,
		.pSampleMask           = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable      = VK_FALSE,
	};

	VkPipelineRenderingCreateInfo pipelineRenderingInfo {
		.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
		.pNext                   = &libraryInfo,
		.viewMask                = 0,
		.colorAttachmentCount    = static_cast<uint32_t>(info.colorFormats.size()),
		.pColorAttachmentFormats = (VkFormat*)info.colorFormats.data(),
		.depthAttachmentFormat   = info.useDepth ? (VkFormat)info.depthFormat : VK_FORMAT_UNDEFINED,
		.stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
	};


	VkGraphicsPipelineCreateInfo pipelineLibraryInfo {
		.sType             = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext             = &pipelineRenderingInfo,
		.flags             = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
		.pMultisampleState = &multisampleState,
		.pColorBlendState  = &colorBlendState,
		.layout            = info.pipelineLayout,
	};

	VkPipeline pipeline;
	auto res = vkCreateGraphicsPipelines(device->handle, device->pipelineCache, 1, &pipelineLibraryInfo, nullptr, &pipeline);
	DEBUG_VK(res, "Failed to create fragment output interface!");
	fragmentOutputInterfaces.emplace(info, pipeline);
}

VkPipeline DeviceResource::PipelineLibrary::LinkPipeline(const std::array<VkPipeline, 4> pipelines, VkPipelineLayout layout) {

	// Link the library parts into a graphics pipeline
	VkPipelineLibraryCreateInfoKHR linkingInfo {
		.sType        = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR,
		.libraryCount = static_cast<uint32_t>(pipelines.size()),
		.pLibraries   = pipelines.data(),
	};

	VkGraphicsPipelineCreateInfo pipelineInfo {
		.sType  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext  = &linkingInfo,
		.layout = layout,
	};

	if (_ctx.linkTimeOptimization) {
		pipelineInfo.flags = VK_PIPELINE_CREATE_LINK_TIME_OPTIMIZATION_BIT_EXT;
	}

	VkPipeline pipeline = VK_NULL_HANDLE;
	auto res = vkCreateGraphicsPipelines(device->handle, device->pipelineCache, 1, &pipelineInfo, nullptr, &pipeline); //todo: Thread pipeline cache
	DEBUG_VK(res, "Failed to create vertex input interface!");
	return pipeline;
}

void DeviceResource::SetDebugUtilsObjectName(const VkDebugUtilsObjectNameInfoEXT& pNameInfo) {
	if (vkSetDebugUtilsObjectNameEXT) {
		vkSetDebugUtilsObjectNameEXT(handle, &pNameInfo);
	}
}

Command::Command() {
	resource = std::make_shared<CommandResource>();
}

void Command::BeginRendering(const RenderingInfo& info) {

	vec4 viewport = info.viewport;
	if (info.viewport == vec4(0.0f)) {
		if (info.colorAttachs.size() > 0) {
			viewport.z = info.colorAttachs[0][0].width;
			viewport.w = info.colorAttachs[0][0].height;
		} else if (info.depthAttach.resource) {
			viewport.z = info.depthAttach.width;
			viewport.w = info.depthAttach.height;
		}
	}

	ivec2 offset(viewport.x, viewport.y);
	uvec2 extent(viewport.z, viewport.w);
	

	std::vector<VkRenderingAttachmentInfoKHR> colorAttachInfos;
	colorAttachInfos.reserve(info.colorAttachs.size());
	for (auto& colorAttach: info.colorAttachs) {
		DEBUG_ASSERT(colorAttach.size() > 0 && colorAttach.size() < 3, "Invalid color attachment count.");
		colorAttachInfos.emplace_back(VkRenderingAttachmentInfoKHR{
			.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
			.imageView   = colorAttach[0].resource->view,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue  = {
				.color   = { 
					.float32 = { 0.1f, 0.1f, 0.1f, 0.0f },
				}
			}
		});
		if (colorAttach.size() > 1) {
			DEBUG_ASSERT(std::min(colorAttach[0].resource->samples, resource->device->physicalDevice->maxSamples) > SampleCount::_1,
				"[Command::BeginRendering] Using MSAA with maxSamples == 1 on device.");
			colorAttachInfos.back().resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
			colorAttachInfos.back().resolveImageView = colorAttach[1].resource->view;
			colorAttachInfos.back().resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}

	VkRenderingInfoKHR renderingInfo{
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
		.flags = 0,
		.renderArea = {
			.offset = { offset.x, offset.y },
			.extent = { extent.x, extent.y },
		},
		.layerCount = info.layerCount,
		.viewMask = 0,
		.colorAttachmentCount = static_cast<uint32_t>(colorAttachInfos.size()),
		.pColorAttachments = colorAttachInfos.data(),
		.pDepthAttachment = nullptr,
		.pStencilAttachment = nullptr,
	};


	VkRenderingAttachmentInfoKHR depthAttachInfo;
	if (info.depthAttach.resource) {
			depthAttachInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
			.imageView = info.depthAttach.resource->view,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = info.depthAttach.usage & ImageUsage::TransientAttachment ? VK_ATTACHMENT_STORE_OP_DONT_CARE: VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue {
				.depthStencil = {
					.depth = 1.0f,
					.stencil = 0
				},
			}
		};
		renderingInfo.pDepthAttachment = &depthAttachInfo;
	}
	
	VkViewport vk_viewport {
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	
	constexpr bool flip_y = true;

	if (flip_y){
		vk_viewport = {
			.x      =  viewport.x,
			.y      =  viewport.w,
			.width  =  viewport.z,
			.height = -viewport.w,
		};
	} else {
		vk_viewport = {
			.x      =  viewport.x,
			.y      =  viewport.y,
			.width  =  viewport.z,
			.height =  viewport.w
		};
	};

	VkRect2D vk_scissor;
	if (info.scissor == ivec4(0)){
		vk_scissor = { offset.x, offset.y, extent.x, extent.y };
	} else {
		vk_scissor = {
			.offset = { info.scissor.x, info.scissor.y },
			.extent = {
				.width  = static_cast<uint32_t>(info.scissor.z),
				.height = static_cast<uint32_t>(info.scissor.w)
			}
		};
	}

	vkCmdSetViewport(resource->buffer, 0, 1, &vk_viewport);
	vkCmdSetScissor(resource->buffer, 0, 1, &vk_scissor);

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

void Command::BindIndexBuffer(Buffer& indexBuffer) {
	vkCmdBindIndexBuffer(resource->buffer, indexBuffer.resource->buffer, 0, VK_INDEX_TYPE_UINT32);
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

// vkWaitForFences + vkResetFences +
// vkResetCommandPool + vkBeginCommandBuffer
void Command::BeginCommandBuffer() {
	vkWaitForFences(resource->device->handle, 1, &resource->fence, VK_TRUE, UINT64_MAX);
	vkResetFences(resource->device->handle, 1, &resource->fence);

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
	vkResetCommandPool(resource->device->handle, resource->pool, 0); // TODO: check if this is needed
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
	auto res = vkQueueWaitIdle(queue->resource->handle);
	DEBUG_VK(res, "Failed to wait idle command buffer");
}

void Command::QueueSubmit(const SubmitInfo& submitInfo) {

	VkSemaphoreSubmitInfo waitInfo {
		.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.semaphore = (VkSemaphore)submitInfo.waitSemaphore,
		.stageMask = (VkPipelineStageFlags2)submitInfo.waitStages
	};
	// waitInfo.value = 1; // Only for timeline semaphores

	VkSemaphoreSubmitInfo signalInfo {
		.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.semaphore = (VkSemaphore)submitInfo.signalSemaphore,
		.stageMask = (VkPipelineStageFlags2)submitInfo.signalStages,
	};

	VkCommandBufferSubmitInfo cmdInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = resource->buffer,
	};

	VkSubmitInfo2 info {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.waitSemaphoreInfoCount = static_cast<uint32_t>(submitInfo.waitSemaphore ? 1 : 0),
		.pWaitSemaphoreInfos = &waitInfo,
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &cmdInfo,
		.signalSemaphoreInfoCount = static_cast<uint32_t>(submitInfo.signalSemaphore ? 1 : 0),
		.pSignalSemaphoreInfos = &signalInfo,
	};

	auto res = vkQueueSubmit2(resource->queue->handle, 1, &info, resource->fence);
	DEBUG_VK(res, "Failed to submit command buffer");
}

void Device::WaitIdle() {
	auto res = vkDeviceWaitIdle(resource->handle);
	DEBUG_VK(res, "Failed to wait idle command buffer");
}



namespace{ // utils
// https://github.com/charles-lunarg/vk-bootstrap/blob/main/src/VkBootstrap.cpp

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
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)         { LOG_INFO ("[Debug Report] {0}", pMessage); };
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)             { LOG_WARN ("[Debug Report] {0}", pMessage); };
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) { LOG_WARN ("[Debug Report Performance] {0}", pMessage); };
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)               { LOG_ERROR("[Debug Report] {0}", pMessage); };
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)               { LOG_TRACE("[Debug Report] {0}", pMessage); };

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
	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = applicationName.c_str(),
		.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.pEngineName = engineName.c_str(),
		.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3,
	};
	
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

	VkInstanceCreateInfo createInfo{
		.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo        = &appInfo,
		.enabledExtensionCount   = static_cast<uint32_t>(activeExtensionsNames.size()),
		.ppEnabledExtensionNames = activeExtensionsNames.data(),
	};

	if (validation_features) {
		static const VkValidationFeatureEnableEXT enableFeatures[] = {
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
			VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
		};
		VkValidationFeaturesEXT validationFeaturesInfo = {
			.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
			.pNext                         = createInfo.pNext,
			.enabledValidationFeatureCount = ARRAY_SIZE(enableFeatures),
			.pEnabledValidationFeatures    = enableFeatures,
		};
		createInfo.pNext = &validationFeaturesInfo;
	}

	// get the name of all extensions that we enabled
	activeExtensionsNames.clear();
	for (size_t i = 0; i < extensionCount; i++) {
		if (activeExtensions[i]) {
			activeExtensionsNames.push_back(instanceExtensions[i].extensionName);
		}
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(activeExtensionsNames.size());
    createInfo.ppEnabledExtensionNames = activeExtensionsNames.data();


	if (_ctx.enableValidationLayers) {
		createInfo.enabledLayerCount = activeLayersNames.size();
		createInfo.ppEnabledLayerNames = activeLayersNames.data();

		// we need to set up a separate logger just for the instance creation/destruction
		// because our "default" logger is created after
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		debugCreateInfo.pNext = createInfo.pNext;
		createInfo.pNext      = &debugCreateInfo;
	}

	// Instance creation
	auto res = vkCreateInstance(&createInfo, _ctx.allocator, &handle);
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
		res = CreateDebugReportCallbackEXT(handle, &debugReportInfo, _ctx.allocator, &debugReport);
		DEBUG_VK(res, "Failed to set up debug report callback!");
		DEBUG_TRACE("Created debug report callback.");
	}

	// LOG_INFO("Created Vulkan Instance.");
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
	
	unusedAttachmentFeatures        = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT, nullptr };
	graphicsPipelineLibraryFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT, &unusedAttachmentFeatures };
	indexingFeatures                = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT,       &graphicsPipelineLibraryFeatures };
	bufferDeviceAddressFeatures     = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,         &indexingFeatures };
	synchronization2Features        = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,         &bufferDeviceAddressFeatures };
	dynamicRenderingFeatures        = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,             &synchronization2Features };
	physicalFeatures2               = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,                             &dynamicRenderingFeatures };
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
	
    // Get max number of samples
    for (VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_64_BIT; sampleCount >= VK_SAMPLE_COUNT_1_BIT; sampleCount = (VkSampleCountFlagBits)(sampleCount >> 1)) {
        if (counts & sampleCount) {
            maxSamples = (SampleCount)sampleCount;
            break;
        }
    }

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
		// Check if queue family is suitable
		bool suitable = ((families[i].queueFlags & desiredFlags) == desiredFlags && ((families[i].queueFlags & undesiredFlags) == 0));
		if (surfaceToSupport != VK_NULL_HANDLE) {
			VkBool32 presentSupport = false;
			VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surfaceToSupport, &presentSupport);
			if (res != VK_SUCCESS) {LOG_ERROR("vkGetPhysicalDeviceSurfaceSupportKHR failed with error code: {}", (uint32_t)res); continue;}
			suitable = suitable && presentSupport;
		}
		if (!suitable) continue; 
		if (avoidIfPossible.empty()) return i;
		// candidate (familyIndex, weight)
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
	// debug print candidates
	for (auto& c: candidates) {
		// DEBUG_TRACE("Candidate: {} ({})", c.first, c.second);
	}
	// Choose candidate with lowest weight
	auto bestFamily = std::min_element(candidates.begin(), candidates.end(), [](auto& l, auto& r) {return l.second < r.second;})->first;
	DEBUG_TRACE("Best family: {}", bestFamily);
	return bestFamily;
}

void DeviceResource::Create(const std::vector<Queue*>& queues) {
	std::unordered_map<uint32_t, uint32_t> queuesToCreate; // <family index, queue count>
	queuesToCreate.reserve(queues.size());
	std::vector<uint32_t> filledUpFamilies; // Track filled up families
	std::unordered_map<GLFWwindow*, VkSurfaceKHR> surfaces; // Surfaces for checking present support

	// Create surfaces for checking present support
	for (auto queue: queues) {
		if (queue->supportedWindowToPresent == nullptr) continue;
		VkSurfaceKHR surface;
		auto res = glfwCreateWindowSurface(_ctx.instance->handle, (GLFWwindow*)queue->supportedWindowToPresent, _ctx.allocator, &surface);
		if (res != VK_SUCCESS) {
			LOG_WARN("Failed to create surface for window: {}", (void*)queue->supportedWindowToPresent);
		} else {
			surfaces.try_emplace((GLFWwindow*)queue->supportedWindowToPresent, surface);
		}
	}
	// Add swapchain extension
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
			// Queue choosing heuristics
			std::span<const std::pair<VkQueueFlags, float>> avoidIfPossible{};
			if(queue->preferSeparateFamily) {
				switch (queue->flags) {
				case VK_QUEUE_COMPUTE_BIT:  avoidIfPossible = {{{VK_QUEUE_GRAPHICS_BIT, 1.0f}, {VK_QUEUE_TRANSFER_BIT, 0.5f}}}; break;
				case VK_QUEUE_TRANSFER_BIT: avoidIfPossible = {{{VK_QUEUE_GRAPHICS_BIT, 1.0f}, {VK_QUEUE_COMPUTE_BIT, 0.5f}}}; break;
				default:                    avoidIfPossible = {{{VK_QUEUE_GRAPHICS_BIT, 1.0f}}}; break;
				}
			}
			// Get family index fitting requirements
			auto familyIndex = physicalDevice.GetQueueFamilyIndex(queue->flags, 0, avoidIfPossible, surfaces[(GLFWwindow*)queue->supportedWindowToPresent], filledUpFamilies);
			if (familyIndex == QUEUE_NOT_FOUND) {
				LOG_WARN("Requested queue flags ({}) not available on device: ({})", (uint32_t)queue->flags, physicalDevice.physicalProperties2.properties.deviceName);
				deviceSuitable = false;
				break;
			} else if (familyIndex == ALL_SUITABLE_QUEUES_TAKEN) {
				LOG_WARN("Requested more queues with flags ({}) than available on device: ({}). Queue was not created", queue->flags, physicalDevice.physicalProperties2.properties.deviceName);
				continue;
			}
			// Create queue
			queue->resource = std::make_shared<QueueResource>(familyIndex, queuesToCreate[familyIndex], queue->flags);
			queuesToCreate[familyIndex]++;
			if (queuesToCreate[familyIndex] == physicalDevice.availableFamilies[familyIndex].queueCount) {
				filledUpFamilies.push_back(familyIndex);
			}
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
	})->queueCount;

	// priority for each type of queue (1.0f for all)
	std::vector<float> priorities(maxQueuesInFamily, 1.0f);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(queuesToCreate.size() * maxQueuesInFamily);
	for (auto &[family, count]: queuesToCreate) {
		queueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{
			.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family,
			.queueCount       = count,
			.pQueuePriorities = &priorities[0],
		});
	}

	auto& supportedFeatures = physicalDevice->physicalFeatures2.features;
	VkPhysicalDeviceFeatures2 features2 {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.features = {
			.geometryShader    = supportedFeatures.geometryShader,
			.sampleRateShading = supportedFeatures.sampleRateShading,
			.logicOp           = supportedFeatures.logicOp,
			.depthClamp        = supportedFeatures.depthClamp,
			.fillModeNonSolid  = supportedFeatures.fillModeNonSolid,
			.wideLines         = supportedFeatures.wideLines,
			.multiViewport     = supportedFeatures.multiViewport,
			.samplerAnisotropy = supportedFeatures.samplerAnisotropy,
		}
	};

	// request features if available
	if (_ctx.usePipelineLibrary) {
		VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT graphicsPipelineLibraryFeatures{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT,
			.pNext = features2.pNext,
			.graphicsPipelineLibrary = physicalDevice->graphicsPipelineLibraryFeatures.graphicsPipelineLibrary,
		}; features2.pNext = &graphicsPipelineLibraryFeatures;
		requiredExtensions.push_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
		requiredExtensions.push_back(VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME);
	}

	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
		.pNext = features2.pNext,
		.shaderUniformBufferArrayNonUniformIndexing   = physicalDevice->indexingFeatures.shaderUniformBufferArrayNonUniformIndexing,
		.shaderSampledImageArrayNonUniformIndexing    = physicalDevice->indexingFeatures.shaderSampledImageArrayNonUniformIndexing,
		.shaderStorageBufferArrayNonUniformIndexing   = physicalDevice->indexingFeatures.shaderStorageBufferArrayNonUniformIndexing,
		.descriptorBindingSampledImageUpdateAfterBind = physicalDevice->indexingFeatures.descriptorBindingSampledImageUpdateAfterBind,
		.descriptorBindingStorageImageUpdateAfterBind = physicalDevice->indexingFeatures.descriptorBindingStorageImageUpdateAfterBind,
		.descriptorBindingPartiallyBound              = physicalDevice->indexingFeatures.descriptorBindingPartiallyBound,
		.runtimeDescriptorArray                       = physicalDevice->indexingFeatures.runtimeDescriptorArray,
	}; features2.pNext = &descriptorIndexingFeatures;

	VkPhysicalDeviceBufferDeviceAddressFeatures addresFeatures{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
		.pNext = features2.pNext,
		.bufferDeviceAddress = physicalDevice->bufferDeviceAddressFeatures.bufferDeviceAddress,
	}; features2.pNext = &addresFeatures;

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

	

	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
		.pNext = features2.pNext,
		.dynamicRendering = physicalDevice->dynamicRenderingFeatures.dynamicRendering,
	}; features2.pNext = &dynamicRenderingFeatures;

	if (_ctx.enableUnusedAttachments) {
		VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT unusedAttachmentFeatures{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT,
			.pNext = features2.pNext,
			.dynamicRenderingUnusedAttachments = physicalDevice->unusedAttachmentFeatures.dynamicRenderingUnusedAttachments,
		}; features2.pNext = &unusedAttachmentFeatures;
		requiredExtensions.push_back(VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME);
	}

	VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
		.pNext = features2.pNext,
		.synchronization2 = physicalDevice->synchronization2Features.synchronization2,
	}; features2.pNext = &sync2Features;

	// VkPhysicalDeviceShaderAtomicFloatFeaturesEXT atomicFeatures{};
	// atomicFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	// atomicFeatures.shaderBufferFloat32AtomicAdd = VK_TRUE;
	// atomicFeatures.pNext = &sync2Features;

	VkDeviceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &features2,
		.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
		.pQueueCreateInfos = queueCreateInfos.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data(),
	};

	// specify the required layers to the device 
	if (_ctx.enableValidationLayers) {
		auto& layers = _ctx.instance->activeLayersNames;
		createInfo.enabledLayerCount = layers.size();
		createInfo.ppEnabledLayerNames = layers.data();
	}

	auto res = vkCreateDevice(physicalDevice->handle, &createInfo, _ctx.allocator, &handle);
	DEBUG_VK(res, "Failed to create logical device!");
	ASSERT(res == VK_SUCCESS, "Failed to create logical device!");
	DEBUG_TRACE("Created logical device");

	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo = {
		.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
				| VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
				//| VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT,
		.physicalDevice = physicalDevice->handle,
		.device = handle,
		.pVulkanFunctions = &vulkanFunctions,
		.instance = _ctx.instance->handle,
		.vulkanApiVersion = VK_API_VERSION_1_3,
	};
	vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator);

	for (auto& q: queues) {
		if (!q->resource) continue; // queue was not created
		auto key = q->resource->familyIndex*maxQueuesInFamily + q->resource->index;
		// auto sh = std::shared_ptr<Queue>(q->resource);
		auto it = uniqueQueues.try_emplace(key, q->resource);
		if (!it.second) {
			LOG_WARN("Queue family index {} index {} already exists", q->resource->familyIndex, q->resource->index);
		}
		vkGetDeviceQueue(handle, q->resource->familyIndex, q->resource->index, &q->resource->handle);
	}

	genericSampler = CreateSampler(handle, {});
	if (_ctx.enableValidationLayers) {
		vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(handle, "vkSetDebugUtilsObjectNameEXT");
	}
	// vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR");
	// vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR");
	// vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR");
	// vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR");
	// vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR");

	VkDescriptorPoolSize imguiPoolSizes[]    = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000} };
	VkDescriptorPoolCreateInfo imguiPoolInfo{};
	imguiPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	imguiPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	imguiPoolInfo.maxSets = (uint32_t)(1024);
	imguiPoolInfo.poolSizeCount = sizeof(imguiPoolSizes)/sizeof(VkDescriptorPoolSize);
	imguiPoolInfo.pPoolSizes = imguiPoolSizes;

	VkResult result = vkCreateDescriptorPool(handle, &imguiPoolInfo, _ctx.allocator, &imguiDescriptorPool);
	DEBUG_VK(result, "Failed to create imgui descriptor pool!");
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

		VkSwapchainCreateInfoKHR createInfo {
		 	.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = surface,
			.minImageCount = imageCount,
			.imageFormat = colorFormat,
			.imageColorSpace = colorSpace,
			.imageExtent = extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT // if we want to render to a separate image first to perform post-processing
							| VK_IMAGE_USAGE_TRANSFER_DST_BIT,// we should change this image usage

			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, // don't support different graphics and present family
			.queueFamilyIndexCount = 0,     // only when imageSharingMode is VK_SHARING_MODE_CONCURRENT
			.pQueueFamilyIndices = nullptr, // only when imageSharingMode is VK_SHARING_MODE_CONCURRENT
			.preTransform = capabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // blend the images with other windows in the window system
			.presentMode = presentMode,
			.clipped = true, // clip pixels behind our window
			.oldSwapchain = VK_NULL_HANDLE,
		};

		auto res = vkCreateSwapchainKHR(device->handle, &createInfo, _ctx.allocator, &swapChain);
		DEBUG_VK(res, "Failed to create swap chain!");
		
		// DEBUG_TRACE("Creating swapchain with {} images.", imageCount);
		vkGetSwapchainImagesKHR(device->handle, swapChain, &imageCount, nullptr);
		imageResources.resize(imageCount);
		vkGetSwapchainImagesKHR(device->handle, swapChain, &imageCount, imageResources.data());
		// DEBUG_TRACE("Created swapchain with {} images.", imageResources.size());
	}

	// create image views
	imageViews.reserve(imageResources.size());
	swapChainImages.clear();
	swapChainImages.reserve(imageResources.size());
	for (auto imageResource: imageResources) {
		VkImageViewCreateInfo viewInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = imageResource,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = colorFormat,
			.subresourceRange {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};
		VkImageView imageView;
		auto res = vkCreateImageView(device->handle, &viewInfo, _ctx.allocator, &imageView);
		DEBUG_VK(res, "Failed to create SwapChain image view!");
		imageViews.push_back(imageView);

		swapChainImages.emplace_back(Image{
			.resource = std::make_shared<ImageResource>(device, imageResource, imageView, true, "SwapChain Image " + std::to_string(swapChainImages.size())),
			.width = width,
			.height = height,
			.layout = ImageLayout::Undefined,
			.aspect = Aspect::Color,
		});
		
	}

    if (_ctx.enableValidationLayers) {
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            std::string strName = ("SwapChain Image " + std::to_string(i));
            device->SetDebugUtilsObjectName({
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE,
                .objectHandle = (uint64_t)(VkImage)imageResources[i],
                .pObjectName = strName.c_str(),
            });
            swapChainImages[i].resource->name = strName;
            device->SetDebugUtilsObjectName({
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE_VIEW,
                .objectHandle = (uint64_t)(VkImageView)imageViews[i],
                .pObjectName = ("SwapChain View " + std::to_string(i)).c_str(),
            });
        }
    }

	if (!is_recreation) {
		// synchronization objects
		imageAvailableSemaphores.resize(framesInFlight);
		renderFinishedSemaphores.resize(framesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		};

		for (size_t i = 0; i < framesInFlight; i++) {
			auto res = vkCreateSemaphore(device->handle, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
			DEBUG_VK(res, "Failed to create semaphore!");
			res = vkCreateSemaphore(device->handle, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
			DEBUG_VK(res, "Failed to create semaphore!");
		}
	}

}

void SwapChainResource::Destroy(bool is_recreation) {
	for (size_t i = 0; i < imageViews.size(); i++) {
		vkDestroyImageView(device->handle, imageViews[i], _ctx.allocator);
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

	imageResources.clear();
	imageViews.clear();
	swapChain = VK_NULL_HANDLE;
}

void SwapChain::Create(Device& device, vkw::Queue& queue, void* glfwWindow, uint32_t width, uint32_t height) {
	this->glfwWindow = glfwWindow;

	resource = std::make_shared<SwapChainResource>();
	resource->device = device.resource.get();

	VkResult res = glfwCreateWindowSurface(_ctx.instance->handle, (GLFWwindow*)glfwWindow, _ctx.allocator, &resource->surface);
	DEBUG_VK(res, "Failed to create window surface!");
	DEBUG_TRACE("Created surface.");

	resource->Create(swapChainImages, width, height, false);

	// command buffers
	resource->commands.resize(framesInFlight);
	for (auto& cmd: resource->commands) {
		cmd.resource->Create(device.resource.get(), queue.resource.get());
	}
	// LOG_INFO("Created Swapchain {}", _ctx.swapChains.size());

	// after commands
	
	currentFrame = 0;
	currentImageIndex = 0;
	dirty = false;
}

void SwapChain::Destroy(){
	for (auto& cmd: resource->commands) {
		vkWaitForFences(resource->device->handle, 1, &cmd.resource->fence, VK_TRUE, UINT_MAX);
	}
	resource->commands.clear();
	swapChainImages.clear();
	resource->Destroy();
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

Command& SwapChain::GetCommandBuffer() {
	return resource->commands[currentFrame];
}

// EndCommandBuffer + vkQueuePresentKHR
void SwapChain::SubmitAndPresent() {

	SubmitInfo submitInfo{};
	submitInfo.waitSemaphore   = (Semaphore*)resource->GetImageAvailableSemaphore(currentFrame);
	submitInfo.signalSemaphore = (Semaphore*)resource->GetRenderFinishedSemaphore(currentFrame);
	
	auto cmd = GetCommandBuffer();
	cmd.EndCommandBuffer();
	cmd.QueueSubmit(submitInfo);

	VkPresentInfoKHR presentInfo{
	 	.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
	 	.waitSemaphoreCount = 1, // TODO: pass with info
	 	.pWaitSemaphores = (VkSemaphore*)&submitInfo.signalSemaphore,
	 	.swapchainCount = 1,
	 	.pSwapchains = &resource->swapChain,
	 	.pImageIndices = &currentImageIndex,
	 	.pResults = nullptr,
	};

	auto res = vkQueuePresentKHR(cmd.resource->queue->handle, &presentInfo); // TODO: use present queue

	switch (res) {
	case VK_SUCCESS: break;
	case VK_ERROR_OUT_OF_DATE_KHR:
	case VK_SUBOPTIMAL_KHR:
		LOG_WARN("vkQueuePresentKHR: {}", VK_ERROR_STRING(res));
		dirty = true;
		return;
	default: DEBUG_VK(res, "Failed to present swap chain image!"); break;
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

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
	 	.sType	       = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
						// | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT
	 	.maxSets	   = 1,
	 	.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
	 	.pPoolSizes    = poolSizes.data(),
	};
	VkResult result = vkCreateDescriptorPool(handle, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
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

	VkResult result = vkCreateDescriptorSetLayout(handle, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
	DEBUG_VK(result, "Failed to allocate descriptor set!");
	ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor set!");
}

void DeviceResource::createDescriptorSet(){
	// Descriptor Set
	VkDescriptorSetAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &descriptorSetLayout,
	};

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
		availableStorageImageRID.resize(MAX_STORAGE_IMAGES);
		std::iota(availableStorageImageRID.rbegin(), availableStorageImageRID.rend(), 0);
		

		VkDescriptorPoolCreateInfo bindlessPoolInfo{
			.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags         = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
			.maxSets       = 1,
			.poolSizeCount = static_cast<uint32_t>(bindlessPoolSizes.size()),
			.pPoolSizes    = bindlessPoolSizes.data(),
		};

		VkResult result = vkCreateDescriptorPool(handle, &bindlessPoolInfo, _ctx.allocator, &bindlessDescriptorPool);
		DEBUG_VK(result, "Failed to create bindless descriptor pool!");
		ASSERT(result == VK_SUCCESS, "Failed to create bindless descriptor pool!");

		// create Descriptor Set Layout for bindless resources
		std::vector<VkDescriptorSetLayoutBinding> bindings {
			{
				.binding = BINDING_TEXTURE,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = MAX_SAMPLEDIMAGES,
				.stageFlags = VK_SHADER_STAGE_ALL,
			}, 
			{
				.binding = BINDING_BUFFER,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = MAX_STORAGE,
				.stageFlags = VK_SHADER_STAGE_ALL,
			},
			// {
			// 	.binding = BINDING_TLAS,
			// 	.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
			// 	.descriptorCount = MAX_ACCELERATIONSTRUCTURE,
			// 	.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			// },
			{
				.binding = BINDING_STORAGE_IMAGE,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
				.descriptorCount = MAX_STORAGE_IMAGES,
				.stageFlags = VK_SHADER_STAGE_ALL,
			},
		};
		
		std::vector<VkDescriptorBindingFlags> bindingFlags {
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT ,
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT ,
			//  VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT ,
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT ,
		};
		// todo: try VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT for last binding ONLY

		VkDescriptorSetLayoutBindingFlagsCreateInfo setLayoutBindingFlagsInfo{
			.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
			.bindingCount  = static_cast<uint32_t>(bindingFlags.size()),
			.pBindingFlags = bindingFlags.data(),
		};

		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{
			.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext        = &setLayoutBindingFlagsInfo,
			.flags        = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
			.bindingCount = static_cast<uint32_t>(bindings.size()),
			.pBindings    = bindings.data(),
		};

		result = vkCreateDescriptorSetLayout(handle, &descriptorLayoutInfo, _ctx.allocator, &bindlessDescriptorLayout);
		DEBUG_VK(result, "Failed to create bindless descriptor set layout!");
		
		// create Descriptor Set for bindless resources
		VkDescriptorSetAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = bindlessDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &bindlessDescriptorLayout,
		};

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
	this->device = device;
	this->queue = queue;
	this->descriptorSet = device->bindlessDescriptorSet;
	VkCommandPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = 0, // do not use VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
		.queueFamilyIndex = this->queue->familyIndex
	};
	
	VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	auto res = vkCreateCommandPool(device->handle, &poolInfo, _ctx.allocator, &pool);
	DEBUG_VK(res, "Failed to create command pool!");

	allocInfo.commandPool = pool;
	res = vkAllocateCommandBuffers(device->handle, &allocInfo, &buffer);
	DEBUG_VK(res, "Failed to allocate command buffer!");

	VkFenceCreateInfo fenceInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};
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



bool Command::Copy(Buffer& dst, void* data, uint32_t size, uint32_t dstOfsset) {
	auto device = resource->device;
	if (device->stagingBufferSize - device->stagingOffset < size) {
		LOG_WARN("not enough size in staging buffer to copy");
		return false;
	}
	// memcpy(resource->stagingCpu + resource->stagingOffset, data, size);
	vmaCopyMemoryToAllocation(device->vmaAllocator, data, device->staging.resource->allocation, device->stagingOffset, size);
	Copy(dst, device->staging, size, dstOfsset, device->stagingOffset);
	DEBUG_TRACE("CmdCopy, size: {}, offset: {}", size, device->stagingOffset);
	device->stagingOffset += size;
	return true;
}

void Command::Copy(Buffer& dst, Buffer& src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset) {
	VkBufferCopy2 copyRegion{
		.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
		.pNext = nullptr,
		.srcOffset = srcOffset,
		.dstOffset = dstOffset,
		.size = size,
	};

	VkCopyBufferInfo2 copyBufferInfo{
		.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
		.srcBuffer = src.resource->buffer,
		.dstBuffer = dst.resource->buffer,
		.regionCount = 1,
		.pRegions = &copyRegion
	};

	vkCmdCopyBuffer2(resource->buffer, &copyBufferInfo);
}

bool Command::Copy(Image& dst, void* data, uint32_t size) {
	auto device = resource->device;
	if (device->stagingBufferSize - device->stagingOffset < size) {
		LOG_WARN("not enough size in staging buffer to copy");
		return false;
	}
	memcpy(device->stagingCpu + device->stagingOffset, data, size);
	Copy(dst, device->staging, device->stagingOffset);
	device->stagingOffset += size;
	return true;
}

void Command::Copy(Image& dst, Buffer& src, uint32_t srcOffset) {
	DEBUG_ASSERT(!(dst.aspect & Aspect::Depth || dst.aspect & Aspect::Stencil), "CmdCopy don't support depth/stencil images");
	VkBufferImageCopy2 region{
		.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
		.pNext = nullptr,
		.bufferOffset = srcOffset,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.imageOffset = { 0, 0, 0 },
		.imageExtent = { dst.width, dst.height, 1 },
	};

	VkCopyBufferToImageInfo2 copyBufferToImageInfo{
		.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
		.srcBuffer = src.resource->buffer,
		.dstImage = dst.resource->image,
		.dstImageLayout = (VkImageLayout)dst.layout,
		.regionCount = 1,
		.pRegions = &region
	};

	vkCmdCopyBufferToImage2(resource->buffer, &copyBufferToImageInfo);
}

void Command::Copy(Buffer &dst, Image &src, uint32_t dstOffset, ivec2 imageOffset, ivec2 imageExtent) {
	DEBUG_ASSERT(!(src.aspect & Aspect::Depth || src.aspect & Aspect::Stencil), "CmdCopy don't support depth/stencil images");
	VkBufferImageCopy2 region{
		.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
		.bufferOffset = dstOffset,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.imageOffset = {imageOffset[0], imageOffset[1], 0},
		.imageExtent = {(uint32_t)imageExtent[0], (uint32_t)imageExtent[1], 1 },
	};

	VkCopyImageToBufferInfo2 copyInfo{
		.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2,
		.srcImage = src.resource->image,
		.srcImageLayout = (VkImageLayout)src.layout,
		.dstBuffer = dst.resource->buffer,
		.regionCount = 1,
		.pRegions = &region,
	};
	vkCmdCopyImageToBuffer2(resource->buffer, &copyInfo);
}

void Command::Barrier(Image& img, const ImageBarrier& barrier) {
	VkImageSubresourceRange range = {
		.aspectMask = (VkImageAspectFlags)img.aspect,
		.baseMipLevel = 0,
		.levelCount = VK_REMAINING_MIP_LEVELS,
		.baseArrayLayer = 0,
		.layerCount = VK_REMAINING_ARRAY_LAYERS,
	};

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
	VkClearColorValue clearColor{{color.x, color.y, color.z, color.w}};
	VkImageSubresourceRange range = {
		.aspectMask = (VkImageAspectFlags)img.aspect,
		.baseMipLevel = 0,
		.levelCount = VK_REMAINING_MIP_LEVELS,
		.baseArrayLayer = 0,
		.layerCount = VK_REMAINING_ARRAY_LAYERS,
	};

	vkCmdClearColorImage(resource->buffer, img.resource->image, (VkImageLayout)img.layout, &clearColor, 1, &range);
}

void Command::Blit(BlitInfo const& info) {	
	auto dst = info.dst;
	auto src = info.src;
	ivec4 dstRegion = info.dstRegion;
	ivec4 srcRegion = info.srcRegion;

	if (info.srcRegion == ivec4{}) {srcRegion = {0, 0, (int)src.width, (int)src.height};}
	if (info.dstRegion == ivec4{}) {dstRegion = {0, 0, (int)dst.width, (int)dst.height};}
	
	VkImageBlit2 blitRegion = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
		.pNext = nullptr,
		.srcSubresource{
			.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel       = 0,
			.baseArrayLayer = 0,
			.layerCount     = 1,
		},
		.srcOffsets = {{srcRegion.x, srcRegion.y, 0}, {srcRegion.z, srcRegion.w, 1}},
		.dstSubresource{
			.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel       = 0,
			.baseArrayLayer = 0,
			.layerCount     = 1,
		},
		.dstOffsets = {{dstRegion.x, dstRegion.y, 0}, {dstRegion.z, dstRegion.w, 1}},
	};

	VkBlitImageInfo2 blitInfo {
		.sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
		.pNext          = nullptr,
		.srcImage       = src.resource->image,
		.srcImageLayout = (VkImageLayout)src.layout,
		.dstImage       = dst.resource->image,
		.dstImageLayout = (VkImageLayout)dst.layout,
		.regionCount    = 1,
		.pRegions       = &blitRegion,
		.filter         = Cast::VkFilter(info.filter),
	};

	vkCmdBlitImage2(resource->buffer, &blitInfo);
}

VkSampler DeviceResource::CreateSampler(VkDevice device, SamplerDesc desc) {
	VkSamplerCreateInfo samplerInfo{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext  = nullptr,
		.flags  = 0,
		.magFilter = Cast::VkFilter(desc.magFilter),
		.minFilter = Cast::VkFilter(desc.minFilter),
		.mipmapMode = Cast::VkSamplerMipmapMode(desc.mipmapMode),
		.addressModeU = Cast::VkSamplerAddressMode(desc.wrapU),
		.addressModeV = Cast::VkSamplerAddressMode(desc.wrapV),
		.addressModeW = Cast::VkSamplerAddressMode(desc.wrapW),
		.mipLodBias = 0.0f,
		
		.anisotropyEnable = desc.maxAnisotropy == 0.0f ? VK_FALSE : physicalDevice->physicalFeatures2.features.samplerAnisotropy,
		.maxAnisotropy = desc.maxAnisotropy,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,

		.minLod = desc.minLod,
		.maxLod = desc.maxLod,

		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
	};

	VkSampler sampler = VK_NULL_HANDLE;
	auto vkRes = vkCreateSampler(handle, &samplerInfo, nullptr, &sampler);
	DEBUG_VK(vkRes, "Failed to create texture sampler!");
	ASSERT(vkRes == VK_SUCCESS, "Failed to create texture sampler!");

	return sampler;
}

size_t Pipeline::Stage::Hash() const {
	size_t hash = 0;
	hash = HashCombine(hash, static_cast<size_t>(stage));
	hash = HashCombine(hash, std::hash<std::string>{}(path.string()));
	hash = HashCombine(hash, std::hash<std::string>{}(entryPoint));
	return hash;
}



void ImGuiCheckVulkanResult(VkResult res) {
    if (res == 0) {
        return;
	}
	LOG_WARN("Imgui error: {}", VK_ERROR_STRING(res));
	DEBUG_VK(res, "Imgui error: {}", VK_ERROR_STRING(res));
}


void SwapChain::CreateUI(SampleCount sampleCount) {
	resource->CreateImGui((GLFWwindow*)glfwWindow, sampleCount);
}

void SwapChainResource::CreateImGui(GLFWwindow* window, SampleCount sampleCount) {
	// if (_ctx.imguiInitialized) return;
	// _ctx.imguiInitialized = true;
	VkFormat colorFormats[] = { VK_FORMAT_R8G8B8A8_UNORM };
	// VkFormat* colorFormats = new VkFormat[] { VK_FORMAT_R8G8B8A8_UNORM };
	VkFormat depthFormat =  VK_FORMAT_D32_SFLOAT;
	ImGui_ImplVulkan_InitInfo initInfo{
		.Instance            = _ctx.instance->handle,
		.PhysicalDevice      = device->physicalDevice->handle,
		.Device              = device->handle,
		.QueueFamily         = commands[0].resource->queue->familyIndex,
		.Queue               = commands[0].resource->queue->handle,
		.DescriptorPool      = device->imguiDescriptorPool,
		.MinImageCount       = surfaceCapabilities.minImageCount,
		.ImageCount          = (uint32_t)imageResources.size(),
		.MSAASamples         = (VkSampleCountFlagBits)std::min(device->physicalDevice->maxSamples, sampleCount),
		.PipelineCache       = device->pipelineCache,
		.UseDynamicRendering = true,
		.PipelineRenderingCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
			.colorAttachmentCount    = 1,
			.pColorAttachmentFormats = colorFormats,
			.depthAttachmentFormat   = depthFormat,
		},
		.Allocator           = _ctx.allocator,
		.CheckVkResultFn     = ImGuiCheckVulkanResult,
	};
	ImGui_ImplGlfw_InitForVulkan(window, true);
	ImGui_ImplVulkan_Init(&initInfo);
}

void ImGuiNewFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void Command::DrawImGui(void* imDrawData) {
	ImGui_ImplVulkan_RenderDrawData(static_cast<ImDrawData*>(imDrawData), resource->buffer);
}

void ImGuiShutdown() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
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
	case VK_RESULT_MAX_ENUM: return "VK_RESULT_MAX_ENUM";
	default:
		if (result < 0)
			return "VK_ERROR_<Unknown>";
		return "VK_<Unknown>";
	}
}
