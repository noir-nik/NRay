#include <Pch.h>
#include <vulkan/vulkan.h>
#include "VulkanBase.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

static const char *VK_ERROR_STRING(VkResult result);
namespace vkw {

struct Context
{
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
	bool enableValidationLayers = false;
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

		VkPhysicalDeviceMemoryProperties memoryProperties;


	// bindless resources
	// VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet bindlessDescriptorSet = VK_NULL_HANDLE;
	VkDescriptorPool bindlessDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout bindlessDescriptorLayout = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;

	void CreateInstance();
	void DestroyInstance();

	void CreatePhysicalDevice();

	void CreateDevice();
	void DestroyDevice();

	// void LoadShaders(Pipeline& pipeline);
    // std::vector<char> CompileShader(const std::filesystem::path& path);
    void CreatePipeline(const PipelineDesc& desc, Pipeline& pipeline);

	// inline CommandResources& GetCurrentCommandResources() {
    //     return queues[currentQueue].commands[swapChainCurrentFrame];
    // }
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

void Init() {
	_ctx.CreateInstance();
	_ctx.CreatePhysicalDevice();
	_ctx.CreateDevice();
	// _ctx.CreateSurfaceFormats();
	// _ctx.CreateSwapChain(width, height);
	// _ctx.CreateImGui(window);
}

void Destroy() {
	// ImGui_ImplVulkan_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// _ctx.DestroySwapChain();
	_ctx.DestroyDevice();
	_ctx.DestroyInstance();
}

// vulkan debug callbacks
namespace {

// utils
namespace{
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
const char* validation_layer_name = "VK_LAYER_KHRONOS_validation";
}

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

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback (
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void*                                       pUserData) {
	// log the message
	// here we can set a minimum severity to log the message
	// if (messageSeverity > VK_DEBUG_UTILS...)
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		LOG_ERROR("[Validation Layer] {0}", pCallbackData->pMessage);
	}
	return VK_FALSE;
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
	createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
}
}

void Context::CreatePipeline(const PipelineDesc& desc, Pipeline& pipeline) {
	pipeline.point = desc.point;
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

	VkPushConstantRange pushConstant{};
	pushConstant.offset = 0;
	pushConstant.size = 256;
	pushConstant.stageFlags = VK_SHADER_STAGE_ALL;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = layouts.size();
	pipelineLayoutInfo.pSetLayouts = layouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

	auto vkRes = vkCreatePipelineLayout(device, &pipelineLayoutInfo, allocator, &pipeline.resource->layout);
	DEBUG_VK(vkRes, "Failed to create pipeline layout!");

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
	} else {
		// graphics pipeline
		ASSERT(0, "Graphics pipeline not implemented");
	}

	for (int i = 0; i < shaderModules.size(); i++) {
		vkDestroyShaderModule(device, shaderModules[i], allocator);
	}
}



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
		bool khronosAvailable = false;
		for (size_t i = 0; i < layers.size(); i++) {
			activeLayers[i] = false;
			if (strcmp("VK_LAYER_KHRONOS_validation", layers[i].layerName) == 0) {
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

		int computeFamily = -1;
		int transferFamily = -1;
		int graphicsFamily = -1;

		// select the family for each type of queue that we want
		for (int i = 0; i < familyCount; i++) {
			auto& family = availableFamilies[i];
			// if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT && graphicsFamily == -1) {
			// 	VkBool32 present = false;
			// 	vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface, &present);
			// 	if (present) {
			// 		graphicsFamily = i;
			// 	}
			// 	continue;
			// }

			if (family.queueFlags & VK_QUEUE_COMPUTE_BIT && computeFamily == -1) {
				computeFamily = i;
				continue;
			}

			if (family.queueFlags & VK_QUEUE_TRANSFER_BIT && transferFamily == -1) {
				transferFamily = i;
				continue;
			}
		}

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
			auto erased = required.erase(std::string(extension.extensionName));
		}
		
		// check if all required queues are supported
		bool suitable = required.empty();
		suitable &= computeFamily != -1;
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

	// VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddresFeatures{};
	// bufferDeviceAddresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	// bufferDeviceAddresFeatures.bufferDeviceAddress = VK_TRUE;
	// bufferDeviceAddresFeatures.pNext = &descriptorIndexingFeatures;

	// VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
	// rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	// rayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
	// rayTracingPipelineFeatures.pNext = &bufferDeviceAddresFeatures;

	// VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};
	// accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	// accelerationStructureFeatures.accelerationStructure = VK_TRUE;
	// accelerationStructureFeatures.descriptorBindingAccelerationStructureUpdateAfterBind = VK_TRUE;
	// accelerationStructureFeatures.accelerationStructureCaptureReplay = VK_TRUE;
	// accelerationStructureFeatures.pNext = &rayTracingPipelineFeatures;

	// VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{};
	// rayQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
	// rayQueryFeatures.rayQuery = VK_TRUE;
	// rayQueryFeatures.pNext = &accelerationStructureFeatures;

	// VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures{};
	// dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	// dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
	// dynamicRenderingFeatures.pNext = &rayQueryFeatures;

	// VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features{};
	// sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
	// sync2Features.synchronization2 = VK_TRUE;
	// sync2Features.pNext = &dynamicRenderingFeatures;

	// VkPhysicalDeviceShaderAtomicFloatFeaturesEXT atomicFeatures{};
	// atomicFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	// atomicFeatures.shaderBufferFloat32AtomicAdd = VK_TRUE;
	// atomicFeatures.pNext = &sync2Features;

	// features2.pNext = &atomicFeatures;
	features2.pNext = &descriptorIndexingFeatures;

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

	// genericSampler = CreateSampler(1.0);
	// vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
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
		// Not needed
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

void Context::DestroyDevice() {
	// dummyVertexBuffer = {};
	// currentPipeline = {};
	// asScratchBuffer = {};
	// vkDestroyDescriptorPool(device, imguiDescriptorPool, allocator);
	// vkDestroyDescriptorPool(device, bindlessDescriptorPool, allocator);
	// vkDestroyDescriptorSetLayout(device, bindlessDescriptorLayout, allocator);
	// bindlessDescriptorSet = VK_NULL_HANDLE;
	// bindlessDescriptorPool = VK_NULL_HANDLE;
	// bindlessDescriptorLayout = VK_NULL_HANDLE;
	vmaDestroyAllocator(vmaAllocator);
	// vkDestroySampler(device, genericSampler, allocator);
	vkDestroyDevice(device, allocator);
	DEBUG_TRACE("Destroyed logical device");
	device = VK_NULL_HANDLE;
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
