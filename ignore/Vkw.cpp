#include "Pch.hpp"
#include <vulkan/vulkan.h>
#include "VulkanBase.hpp"

#include "ShaderCommon.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

static const char *VK_ERROR_STRING(VkResult result);
namespace vkw {

struct Context
{	
	void CmdCopy(Buffer& dst, void* data, uint32_t size, uint32_t dstOfsset);
    void CmdCopy(Buffer& dst, Buffer& src, uint32_t size, uint32_t dstOffset, uint32_t srcOffset);
    void CmdCopy(Image& dst, void* data, uint32_t size);
    void CmdCopy(Image& dst, Buffer& src, uint32_t size, uint32_t srcOffset);
    void CmdBarrier(Image& img, Layout::ImageLayout layout);
    void CmdBarrier();
    void EndCommandBuffer(VkSubmitInfo submitInfo);

    void LoadShaders(Pipeline& pipeline);
    std::vector<char> CompileShader(const std::filesystem::path& path);
    void CreatePipeline(const PipelineDesc& desc, Pipeline& pipeline);

	VkInstance instance = VK_NULL_HANDLE;
	// VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
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
	bool enableValidationLayers = true;
	// bool enableValidationLayers = false;
	std::vector<bool> activeLayers; // Available layers
	std::vector<const char*> activeLayersNames;
	std::vector<VkLayerProperties> layers;
	std::vector<bool> activeExtensions;                    // Instance Extensions
	std::vector<const char*> activeExtensionsNames;        // Instance Extensions
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
		// VkSampleCountFlagBits maxSamples = VK_SAMPLE_COUNT_1_BIT;
		// VkSampleCountFlags sampleCounts;

		VkPhysicalDeviceFeatures physicalFeatures{};
		// VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		VkPhysicalDeviceProperties physicalProperties{};
		
		// std::vector<VkPresentModeKHR> availablePresentModes;
		// std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;
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
			VkQueue queue = VK_NULL_HANDLE;
			int family = -1;
			std::vector<CommandResources> commands;
		};
		InternalQueue queues[Queue::Count];
		Queue currentQueue = Queue::Count;
		std::shared_ptr<PipelineResource> currentPipeline;
		// const uint32_t stagingBufferSize = 256 * 1024 * 1024;
		const uint32_t stagingBufferSize = 8 * 1024 * 1024;

		VkPhysicalDeviceMemoryProperties memoryProperties;


	// bindless resources
	// VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet bindlessDescriptorSet = VK_NULL_HANDLE;
	VkDescriptorPool bindlessDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout bindlessDescriptorLayout = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;

	// VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    // std::vector<Image> swapChainImages;
    // std::vector<VkImage> swapChainImageResources;
    // std::vector<VkImageView> swapChainViews;
    // std::vector<VkSemaphore> imageAvailableSemaphores;
    // std::vector<VkSemaphore> renderFinishedSemaphores;

    // uint32_t additionalImages = 0;
    uint32_t framesInFlight = 1;
    // VkFormat depthFormat;
    // VkExtent2D swapChainExtent;
    uint32_t swapChainCurrentFrame = 0; // 0 For compute
    // bool swapChainDirty = true;
    // uint32_t currentImageIndex = 0;

	std::vector<int32_t> availableBufferRID;
    std::vector<int32_t> availableImageRID;
    std::vector<int32_t> availableTLASRID;
	VkSampler genericSampler;

	// // preferred, warn if not available
    // VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
    // VkColorSpaceKHR colorSpace  = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    // VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    // VkSampleCountFlagBits numSamples  = VK_SAMPLE_COUNT_1_BIT;

	const uint32_t initialScratchBufferSize = 64*1024*1024;
    Buffer asScratchBuffer;
    VkDeviceAddress asScratchAddress;
	Buffer dummyVertexBuffer;


    std::map<std::string, float> timeStampTable;

	void CreateInstance();
	void DestroyInstance();

	void CreatePhysicalDevice();

	void CreateDevice();
	void DestroyDevice();

	// void CreateSurfaceFormats();

    // void CreateSwapChain(uint32_t width, uint32_t height);
    // void DestroySwapChain();

	void createCommandBuffers();
	void DestroyCommandBuffers();

	uint32_t FindMemoryType(uint32_t type, VkMemoryPropertyFlags properties);
	bool SupportFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);


	// inline Image& GetCurrentSwapChainImage() {
    //     return swapChainImages[currentImageIndex];
    // }

	inline CommandResources& GetCurrentCommandResources() {
        return queues[currentQueue].commands[swapChainCurrentFrame];
    }

	// VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
    // VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    // VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkSampler CreateSampler(float maxLod);

    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
    // PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
    // PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
    // PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
    // PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
    // PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
    // PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
};
static Context _ctx;

void Context::CreateInstance(){
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
		for (size_t i = 0; i < layers.size(); i++) {
			activeLayers[i] = false;
			if (strcmp(validation_layer_name, layers[i].layerName) == 0) {
				activeLayers[i] = true;
				khronosAvailable = true;
				break;
			}
		}
		if (!khronosAvailable) {LOG_ERROR("Default validation layer not available!");}

		for (size_t i = 0; i < layers.size(); i++) { 
			if (activeLayers[i]) {
				activeLayersNames.push_back(layers[i].layerName);
			}
		}
	}
	
	// get all extensions required by glfw
	// uint32_t glfwExtensionCount = 0;
	// const char** glfwExtensions;
	// glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	// auto requiredInstanceExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	auto requiredInstanceExtensions = std::vector<const char*>(); // Only compute

	// Extensions
	if (enableValidationLayers) {
		requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	// if (!requiredInstanceExtensions.empty()) {
		// get all available extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 0);
		instanceExtensions.resize(extensionCount);
		activeExtensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

		// set to active all extensions that we enabled
		for (size_t i = 0; i < requiredInstanceExtensions.size(); i++) {
			for (size_t j = 0; j < instanceExtensions.size(); j++) {
				if (strcmp(requiredInstanceExtensions[i], instanceExtensions[j].extensionName) == 0) {
					activeExtensions[j] = true;
					break;
				}
			}
		}
		
		// get the name of all extensions that we enabled
		activeExtensionsNames.clear();
		for (size_t i = 0; i < instanceExtensions.size(); i++) {
			if (activeExtensions[i]) {
				activeExtensionsNames.push_back(instanceExtensions[i].extensionName);
			}
		}
	// }

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
		// createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
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
	
	if (enableValidationLayers) {
		VkDebugUtilsMessengerCreateInfoEXT messengerInfo;
		PopulateDebugMessengerCreateInfo(messengerInfo);
		res = CreateDebugUtilsMessengerEXT(instance, &messengerInfo, allocator, &debugMessenger);
		DEBUG_VK(res, "Failed to set up debug messenger!");
		DEBUG_TRACE("Created debug messenger.");
	}

	// res = glfwCreateWindowSurface(instance, glfwWindow, allocator, &surface);
	// DEBUG_VK(res, "Failed to create window surface!");
	// DEBUG_TRACE("Created surface.");

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
	// vkDestroySurfaceKHR(instance, surface, allocator);
	// DEBUG_TRACE("Destroyed surface.");
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

		int computeFamily = -1; // TODO: change to uint32_t
		int transferFamily = -1; // and use VK_QUEUE_FAMILY_IGNORED ?
		int graphicsFamily = -1;

		// select the family for each type of queue that we want
		for (int i = 0; i < familyCount; i++) {
			auto& family = availableFamilies[i];
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT && graphicsFamily == -1) {
				// VkBool32 present = false;
				// vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface, &present);
				// if (present) {
					graphicsFamily = i;
				// }
				continue;
			}

			if (family.queueFlags & VK_QUEUE_COMPUTE_BIT && computeFamily == -1) {
				computeFamily = i;
				continue;
			}

			if (family.queueFlags & VK_QUEUE_TRANSFER_BIT && transferFamily == -1) {
				transferFamily = i;
				continue;
			}
		}

		// TODO: change logic
		queues[Queue::Graphics].family = graphicsFamily;
		queues[Queue::Compute].family = computeFamily == -1 ? graphicsFamily : computeFamily;
		queues[Queue::Transfer].family = transferFamily == -1 ? graphicsFamily : transferFamily;

		// get max number of samples
		// TODO: replace with vkGetPhysicalDeviceFeatures2
		vkGetPhysicalDeviceFeatures(device, &physicalFeatures);
		vkGetPhysicalDeviceProperties(device, &physicalProperties);
		vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

		// VkSampleCountFlags counts = physicalProperties.limits.framebufferColorSampleCounts;
		// counts &= physicalProperties.limits.framebufferDepthSampleCounts;

		// maxSamples = VK_SAMPLE_COUNT_1_BIT;
		// if (counts & VK_SAMPLE_COUNT_64_BIT) { maxSamples = VK_SAMPLE_COUNT_64_BIT; }
		// else if (counts & VK_SAMPLE_COUNT_32_BIT) { maxSamples = VK_SAMPLE_COUNT_32_BIT; }
		// else if (counts & VK_SAMPLE_COUNT_16_BIT) { maxSamples = VK_SAMPLE_COUNT_16_BIT; }
		// else if (counts & VK_SAMPLE_COUNT_8_BIT) { maxSamples = VK_SAMPLE_COUNT_8_BIT; }
		// else if (counts & VK_SAMPLE_COUNT_4_BIT) { maxSamples = VK_SAMPLE_COUNT_4_BIT; }
		// else if (counts & VK_SAMPLE_COUNT_2_BIT) { maxSamples = VK_SAMPLE_COUNT_2_BIT; }

		// check if all required extensions are available
		std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());
		for (const auto& extension : availableExtensions) {
			required.erase(std::string(extension.extensionName));
		}
		
		LOG_INFO("Families: {0}, {1}, {2}", graphicsFamily, computeFamily, transferFamily);
		// check if all required queues are supported
		bool suitable = required.empty();
		if (!suitable) LOG_ERROR("Required extensions not available: {0}", required.begin()->c_str());
		suitable &= graphicsFamily != -1;
		if (!suitable) LOG_ERROR("Required compute queue not available");
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
	
	std::set<uint32_t> uniqueFamilies;
	for (int q = 0; q < Queue::Count; q++) {
		uniqueFamilies.emplace(queues[q].family);
	};

	// priority for each type of queue
	float priority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (uint32_t family : uniqueFamilies) {
		VkDeviceQueueCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = family;
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = &priority;
		queueCreateInfos.push_back(createInfo);
	}

	auto supportedFeatures = physicalFeatures;

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

	// add to member and as pNext of physicalFeatures
	// to check with vkGetPhysicalDeviceFeatures2
	// ref: https://dev.to/gasim/implementing-bindless-design-in-vulkan-34no
	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
	descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	descriptorIndexingFeatures.runtimeDescriptorArray = true;
	descriptorIndexingFeatures.descriptorBindingPartiallyBound = true;
	descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;
	descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing = true;
	descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = true;
	descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = true;
	descriptorIndexingFeatures.descriptorBindingStorageImageUpdateAfterBind = true;

	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddresFeatures{};
	bufferDeviceAddresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	bufferDeviceAddresFeatures.bufferDeviceAddress = VK_TRUE;
	bufferDeviceAddresFeatures.pNext = &descriptorIndexingFeatures;

	VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features{};
    sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    sync2Features.synchronization2 = VK_TRUE;
    sync2Features.pNext = &bufferDeviceAddresFeatures;

	// Add other features to chain here

	features2.pNext = &sync2Features;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.pEnabledFeatures; // !Should be NULL if pNext is used
	createInfo.pNext = &features2; // feature chain
	// VkPhysicalDeviceFeatures deviceFeatures{}; // None just yet
	// createInfo.pEnabledFeatures = &deviceFeatures;

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

	for (int q = 0; q < Queue::Count; q++) {
		vkGetDeviceQueue(device, queues[q].family, 0, &queues[q].queue);
	}

	genericSampler = CreateSampler(1.0);
	vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
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

	// create bindless resources
	{

		// TODO: val = min(MAX_, available)
		const u32 MAX_STORAGE = 8192;
        const u32 MAX_SAMPLEDIMAGES = 8192;
        const u32 MAX_ACCELERATIONSTRUCTURE = 64;
        const u32 MAX_STORAGE_IMAGES = 8192;

		// create descriptor set pool for bindless resources
        std::vector<VkDescriptorPoolSize> bindlessPoolSizes = { 
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_SAMPLEDIMAGES},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAX_STORAGE},
            {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, MAX_ACCELERATIONSTRUCTURE},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, MAX_STORAGE_IMAGES},
        };

		// Fill with sequential numbers
		availableBufferRID.resize(MAX_STORAGE);
		std::iota(availableBufferRID.begin(), availableBufferRID.end(), 0);
		availableImageRID.resize(MAX_SAMPLEDIMAGES);
		std::iota(availableImageRID.begin(), availableImageRID.end(), 0);
		availableTLASRID.resize(MAX_ACCELERATIONSTRUCTURE);
		std::iota(availableTLASRID.begin(), availableTLASRID.end(), 0);

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

		VkDescriptorSetLayoutBinding accelerationStructureBinding{};
        accelerationStructureBinding.binding = BINDING_TLAS;
        accelerationStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        accelerationStructureBinding.descriptorCount = MAX_ACCELERATIONSTRUCTURE;
        accelerationStructureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(accelerationStructureBinding);
        bindingFlags.push_back({ VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT });

        VkDescriptorSetLayoutBinding imageStorageBinding{};
        imageStorageBinding.binding = BINDING_STORAGE_IMAGE;
        imageStorageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        imageStorageBinding.descriptorCount = MAX_STORAGE_IMAGES;
        imageStorageBinding.stageFlags = VK_SHADER_STAGE_ALL;
        bindings.push_back(imageStorageBinding);
        bindingFlags.push_back({ VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT });


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

	asScratchBuffer = vkw::CreateBuffer(initialScratchBufferSize, vkw::BufferUsage::Address | vkw::BufferUsage::Storage, vkw::Memory::GPU);
	VkBufferDeviceAddressInfo scratchInfo{};
	scratchInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	scratchInfo.buffer = asScratchBuffer.resource->buffer;
	asScratchAddress = vkGetBufferDeviceAddress(device, &scratchInfo);

	dummyVertexBuffer = vkw::CreateBuffer(
		6 * 3 * sizeof(float),
		vkw::BufferUsage::Vertex | vkw::BufferUsage::AccelerationStructureInput,
		vkw::Memory::GPU,
		"VertexBuffer#Dummy"
	);
}

void Context::DestroyDevice() {
	dummyVertexBuffer = {};
	currentPipeline = {};
	asScratchBuffer = {};
	// vkDestroyDescriptorPool(device, imguiDescriptorPool, allocator);
	vkDestroyDescriptorPool(device, bindlessDescriptorPool, allocator);
	vkDestroyDescriptorSetLayout(device, bindlessDescriptorLayout, allocator);
	bindlessDescriptorSet = VK_NULL_HANDLE;
	bindlessDescriptorPool = VK_NULL_HANDLE;
	bindlessDescriptorLayout = VK_NULL_HANDLE;
	vmaDestroyAllocator(vmaAllocator);
	vkDestroySampler(device, genericSampler, allocator);
	vkDestroyDevice(device, allocator);
	DEBUG_TRACE("Destroyed logical device");
	device = VK_NULL_HANDLE;
}

void Context::createCommandBuffers(){
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = 0; // ?VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	for (int q = 0; q < Queue::Count; q++) {
		InternalQueue& queue = queues[q];
		if (queue.family == -1) { //TODO:
			ASSERT(0, "Queue family = -1, fix in CreatePhysicalDevice()");
		}
		poolInfo.queueFamilyIndex = queue.family;
		queue.commands.resize(framesInFlight);
		for (int i = 0; i < framesInFlight; i++) {
			auto res = vkCreateCommandPool(device, &poolInfo, allocator, &queue.commands[i].pool);
			DEBUG_VK(res, "Failed to create command pool!");

			allocInfo.commandPool = queue.commands[i].pool;
			res = vkAllocateCommandBuffers(device, &allocInfo, &queue.commands[i].buffer);
			DEBUG_VK(res, "Failed to allocate command buffer!");

			queue.commands[i].staging = CreateBuffer(stagingBufferSize, BufferUsage::TransferSrc, Memory::CPU, "StagingBuffer" + std::to_string(q) + "_" + std::to_string(i));
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
	}
}

void Context::DestroyCommandBuffers() {
	for (int q = 0; q < Queue::Count; q++) {
		for (int i = 0; i < framesInFlight; i++) {
			// vkFreeCommandBuffers(device, queues[q].commands[i].pool, 1, &queues[q].commands[i].buffer); // No OP
            vkDestroyCommandPool(device, queues[q].commands[i].pool, allocator);
            queues[q].commands[i].staging = {};
            queues[q].commands[i].stagingCpu = nullptr;
            vkDestroyFence(device, queues[q].commands[i].fence, allocator);
            // vkDestroyQueryPool(device, queues[q].commands[i].queryPool, allocator);
        }
	}
}



}