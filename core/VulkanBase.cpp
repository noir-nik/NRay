#include <Pch.h>
#include <vulkan/vulkan.h>
#include "VulkanBase.h"

namespace vkw {

struct Context
{
	VkInstance instance;
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
	std::vector<bool> activeLayers;
	std::vector<VkLayerProperties> layers;
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	// std::vector<const char*> requiredExtensions;


	// VkPhysicalDeviceFeatures physicalFeatures{};
	// VkPhysicalDeviceProperties physicalProperties{};

	// std::vector<VkExtensionProperties> availableExtensions;
	// std::vector<VkQueueFamilyProperties> availableFamilies;

	// bindless resources
	// VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
	// VkDescriptorSet bindlessDescriptorSet = VK_NULL_HANDLE;
	// VkDescriptorPool bindlessDescriptorPool = VK_NULL_HANDLE;
	// VkDescriptorSetLayout bindlessDescriptorLayout = VK_NULL_HANDLE;

	void CreateInstance();
};
static Context _ctx;


void Init() {
	_ctx.CreateInstance();
	// _ctx.CreatePhysicalDevice();
	// _ctx.CreateDevice();
	// _ctx.CreateSurfaceFormats();
	// _ctx.CreateSwapChain(width, height);
	// _ctx.CreateImGui(window);
}

VkResult CreateDebugUtilsMessengerEXT (
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
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
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
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
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
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

void Context::CreateInstance(){
	if (enableValidationLayers) {
		// get all available layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		layers.resize(layerCount);
		activeLayers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		// get all available extensions
		// uint32_t extensionCount = 0;
		// vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 0);
		// instanceExtensions.resize(extensionCount);
		// activeExtensions.resize(extensionCount);
		// vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

		// get api version
		vkEnumerateInstanceVersion(&apiVersion);

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

		if (enableValidationLayers && !khronosAvailable) { 
			LOG_ERROR("Default validation layer not available!");
		}
	}

}

}