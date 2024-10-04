#include <Pch.h>
#include <vulkan/vulkan.h>
#include "VulkanBase.h"

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

	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	uint32_t presentQueueFamilyIndex;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkCommandPool commandPool;
	VkCommandBuffer *commandBuffers;
	VkFence *fences;

	uint32_t apiVersion;
	bool enableValidationLayers = true;
    std::vector<bool> activeLayers; // Available layers
    std::vector<const char*> activeLayersNames;
    std::vector<VkLayerProperties> layers;
    std::vector<bool> activeExtensions; // API Extensions
    std::vector<const char*> activeExtensionsNames;
    std::vector<VkExtensionProperties> instanceExtensions;
	std::vector<const char*> requiredExtensions = {
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
    std::vector<VkExtensionProperties> availableExtensions;
    std::vector<VkQueueFamilyProperties> availableFamilies;

	struct InternalQueue {
		VkQueue queue = VK_NULL_HANDLE;
		int family = -1;
		// std::vector<CommandResources> commands;
	};
	InternalQueue queues[Queue::Count];
	Queue currentQueue = Queue::Count;

	VkPhysicalDeviceMemoryProperties memoryProperties;


	// bindless resources
	// VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
	// VkDescriptorSet bindlessDescriptorSet = VK_NULL_HANDLE;
	// VkDescriptorPool bindlessDescriptorPool = VK_NULL_HANDLE;
	// VkDescriptorSetLayout bindlessDescriptorLayout = VK_NULL_HANDLE;

	void CreateInstance();
	void DestroyInstance();

	void CreatePhysicalDevice();

	void CreateDevice();
	void DestroyDevice();
};
static Context _ctx;


void Init() {
	_ctx.CreateInstance();
	_ctx.CreatePhysicalDevice();
	// _ctx.CreateDevice();
	// _ctx.CreateSurfaceFormats();
	// _ctx.CreateSwapChain(width, height);
	// _ctx.CreateImGui(window);
}

void Destroy() {
    // _ctx.DestroyDevice();
    _ctx.DestroyInstance();
}

// vulkan debug callbacks
namespace {
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
	
	// Extensions
	if (enableValidationLayers) {
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (!requiredExtensions.empty()) {
		// get all available extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 0);
		instanceExtensions.resize(extensionCount);
		activeExtensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

		// set to active all extensions that we enabled
		for (size_t i = 0; i < requiredExtensions.size(); i++) {
			for (size_t j = 0; j < instanceExtensions.size(); j++) {
				if (strcmp(requiredExtensions[i], instanceExtensions[j].extensionName) == 0) {
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
        // createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

	// Instance creation
	auto res = vkCreateInstance(&createInfo, allocator, &instance);
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

		// check if all required queues are supported
		bool suitable = required.empty();
		// suitable &= graphicsFamily != -1;
		suitable &= computeFamily != -1;

		if (suitable) {
			physicalDevice = device;
			break;
		}
	}

	ASSERT(physicalDevice != VK_NULL_HANDLE, "no device with Vulkan support!");
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
