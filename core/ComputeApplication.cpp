#include "Pch.hpp"

#include <vector>
#include <string.h>
#include <assert.h>
#include <stdexcept>
#include <cmath>
#include <iostream>

#include "vk_utils.h"
#include "FileManager.hpp"
#include "ComputeApplication.hpp"


#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256

const int WIDTH		= SCREEN_WIDTH;
const int HEIGHT	 = SCREEN_HEIGHT;
const int WORKGROUP_SIZE = 16;	
constexpr int num_push_constants = 4;
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace vkw{
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
	std::vector<bool> activeExtensions;			// Instance Extensions
	std::vector<const char*> activeExtensionsNames;	// Instance Extensions
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
	// VkColorSpaceKHR colorSpace	= VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	// VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	// VkSampleCountFlagBits numSamples	= VK_SAMPLE_COUNT_1_BIT;

	// const uint32_t initialScratchBufferSize = 64*1024*1024;
	// Buffer asScratchBuffer;
	// VkDeviceAddress asScratchAddress;
	// Buffer dummyVertexBuffer;


	// std::map<std::string, float> timeStampTable;

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
	//	 return swapChainImages[currentImageIndex];
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

}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
	VkDebugReportFlagsEXT				 flags,
	VkDebugReportObjectTypeEXT			objectType,
	uint64_t					object,
	size_t						location,
	int32_t					 messageCode,
	const char*				 pLayerPrefix,
	const char*				 pMessage,
	void*						 pUserData)
{
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}

void ComputeApplication::run()	{
	const int deviceId = 0;
	std::cout << "init vulkan for device " << deviceId << " ... " << std::endl;
	instance = vk_utils::CreateInstance(enableValidationLayers, enabledLayers);
	if(enableValidationLayers)
	{
	vk_utils::InitDebugReportCallback(instance,
					&debugReportCallbackFn, &debugReportCallback);
	}
	physicalDevice = vk_utils::FindPhysicalDevice(instance, true, deviceId);
	uint32_t queueFamilyIndex = vk_utils::GetComputeQueueFamilyIndex(physicalDevice);
	device = vk_utils::CreateLogicalDevice(queueFamilyIndex, physicalDevice, enabledLayers);
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
	
	size_t bufferSize = sizeof(Pixel) * WIDTH * HEIGHT;
	std::cout << "creating resources ... " << std::endl;
	//pixels
	createBufferApp(device, physicalDevice, bufferSize,	
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,	
		bufferPixels, bufferMemoryPixels);	 

	
	//Staging buffer
	size_t weightsBufferSize = mlp.w_b.size() * sizeof(float);//////TODO		
	createBufferApp(device, physicalDevice, weightsBufferSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			bufferStaging, bufferMemoryStaging);

	//Weights buffer on device
	createBufferApp(device, physicalDevice, weightsBufferSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			bufferWeightsDevice, bufferMemoryWeightsDevice);

	//copy weights
	void* data;
	vkMapMemory(device, bufferMemoryStaging, 0, weightsBufferSize, 0, &data);
	memcpy(data, mlp.w_b.data(), (size_t)weightsBufferSize);
	vkUnmapMemory(device, bufferMemoryStaging);

	


	createDescriptorSetLayout(device, &descriptorSetLayout);	
	//pixels + weights	 
			
				
	createDescriptorSetFor_2_Buffers(device, bufferPixels, bufferSize,
					bufferWeightsDevice, weightsBufferSize,
					&descriptorSetLayout, 
					&descriptorPool, &descriptorSet);		 
	std::cout << "compiling shaders	... " << std::endl;
	createComputePipeline(device, descriptorSetLayout,
			&computeShaderModule, &pipeline, &pipelineLayout);
	createCommandBuffer(device, queueFamilyIndex, pipeline, pipelineLayout,
			&commandPool, &commandBuffer);
	// DO
	// copy to device
	copyBuffer(bufferStaging, bufferWeightsDevice, weightsBufferSize, queue);


	recordCommandsTo(commandBuffer, pipeline, pipelineLayout, descriptorSet);
	
	std::cout << "doing computations ... " << std::endl;
	runCommandBuffer(commandBuffer, queue, device);
	
	
	std::cout << "saving image		 ... " << std::endl;
	saveRenderedImageFromDeviceMemory(device, bufferMemoryPixels, 0, WIDTH, HEIGHT);
	
	std::cout << "destroying all	 ... " << std::endl;
	cleanup();
}

	
	
void ComputeApplication::saveRenderedImageFromDeviceMemory(VkDevice a_device, VkDeviceMemory a_bufferMemory, size_t a_offset, int a_width, int a_height)
	{
		const int a_bufferSize = a_width * sizeof(Pixel);
		void* mappedMemory = nullptr;
		
		
		std::vector<unsigned char> image;
		image.reserve(a_width * a_height * 4);
		for (int i = 0; i < a_height; i += 1) 
		{
	size_t offset = a_offset + i * a_width * sizeof(Pixel);
	mappedMemory = nullptr;
	
	vkMapMemory(a_device, a_bufferMemory, offset, a_bufferSize, 0, &mappedMemory);
	Pixel* pmappedMemory = (Pixel *)mappedMemory;
	for (int j = 0; j < a_width; j += 1)
	{
		image.push_back((unsigned char)(255.0f * (pmappedMemory[j].r)));
		image.push_back((unsigned char)(255.0f * (pmappedMemory[j].g)));
		image.push_back((unsigned char)(255.0f * (pmappedMemory[j].b)));
		image.push_back((unsigned char)(255.0f * (pmappedMemory[j].a)));
	}
	
	vkUnmapMemory(a_device, a_bufferMemory);
		}
		FileManager::SaveBMP("out_gpu.bmp", (const uint32_t*)image.data(), WIDTH, HEIGHT);
	}

	

void ComputeApplication::createBufferApp(VkDevice a_device, VkPhysicalDevice a_physDevice, 
				VkDeviceSize a_size, VkBufferUsageFlags a_usage, VkMemoryPropertyFlags a_properties,
				 VkBuffer& a_buffer, VkDeviceMemory& a_bufferMemory) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = a_size;
		bufferInfo.usage = a_usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_RESULT(vkCreateBuffer(a_device, &bufferInfo, NULL, &a_buffer)); 

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(a_device, a_buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = vk_utils::FindMemoryType(memRequirements.memoryTypeBits, a_properties, a_physDevice);

		VK_CHECK_RESULT(vkAllocateMemory(a_device, &allocInfo, nullptr, &a_bufferMemory));
		

		VK_CHECK_RESULT(vkBindBufferMemory(a_device, a_buffer, a_bufferMemory, 0));
	}

void ComputeApplication::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkQueue queue) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer_local;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer_local);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer_local, &beginInfo);
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer_local, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer_local);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer_local;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer_local);
}
	
	// pixels + second buffer
void ComputeApplication::createDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout)
	{
		 VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[2];
		 descriptorSetLayoutBinding[0].binding		= 0;
		 descriptorSetLayoutBinding[0].descriptorType	 = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		 descriptorSetLayoutBinding[0].descriptorCount	= 1;
		 descriptorSetLayoutBinding[0].stageFlags	 = VK_SHADER_STAGE_COMPUTE_BIT;

		 descriptorSetLayoutBinding[1].binding		= 1;
		 descriptorSetLayoutBinding[1].descriptorType	 = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		 descriptorSetLayoutBinding[1].descriptorCount	= 1;
		 descriptorSetLayoutBinding[1].stageFlags	 = VK_SHADER_STAGE_COMPUTE_BIT;

		 VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		 descriptorSetLayoutCreateInfo.sType	= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		 descriptorSetLayoutCreateInfo.bindingCount = 2; 
		 descriptorSetLayoutCreateInfo.pBindings	= descriptorSetLayoutBinding;
		 
		 VK_CHECK_RESULT(vkCreateDescriptorSetLayout(a_device, &descriptorSetLayoutCreateInfo, NULL, a_pDSLayout));
	}
	
void ComputeApplication::createDescriptorSetFor_2_Buffers(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize,
						VkBuffer a_secondBuffer, size_t a_secondBufferSize,
						const VkDescriptorSetLayout* a_pDSLayout,
						VkDescriptorPool* a_pDSPool, VkDescriptorSet* a_pDS)
	{
		VkDescriptorPoolSize descriptorPoolSize[2];
		descriptorPoolSize[0].type		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorPoolSize[0].descriptorCount = 1;
		descriptorPoolSize[1].type		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorPoolSize[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType	 = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets		 = 1; 
		descriptorPoolCreateInfo.poolSizeCount = 2;
		descriptorPoolCreateInfo.pPoolSizes	= descriptorPoolSize;
		VK_CHECK_RESULT(vkCreateDescriptorPool(a_device, &descriptorPoolCreateInfo, NULL, a_pDSPool));

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool	 = (*a_pDSPool); 
		descriptorSetAllocateInfo.descriptorSetCount = 1;		
		descriptorSetAllocateInfo.pSetLayouts	= a_pDSLayout;
		VK_CHECK_RESULT(vkAllocateDescriptorSets(a_device, &descriptorSetAllocateInfo, a_pDS));
		
		//pixels
		VkDescriptorBufferInfo descriptorBufferInfo = {};
		descriptorBufferInfo.buffer = a_buffer;
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range	= a_bufferSize;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType		 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet		= (*a_pDS); 
		writeDescriptorSet.dstBinding		= 0;	
		writeDescriptorSet.descriptorCount = 1;	
		writeDescriptorSet.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; 
		writeDescriptorSet.pBufferInfo	 = &descriptorBufferInfo;
		vkUpdateDescriptorSets(a_device, 1, &writeDescriptorSet, 0, NULL);

		//second buffer
		VkDescriptorBufferInfo descriptorBufferInfo2 = {};
		descriptorBufferInfo2.buffer = a_secondBuffer;
		descriptorBufferInfo2.offset = 0;
		descriptorBufferInfo2.range	= a_secondBufferSize;
		
		VkWriteDescriptorSet writeDescriptorSet2 = {};
		writeDescriptorSet2.sType		 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet2.dstSet		= (*a_pDS); 
		writeDescriptorSet2.dstBinding		= 1;	
		writeDescriptorSet2.descriptorCount = 1;	
		writeDescriptorSet2.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; 
		writeDescriptorSet2.pBufferInfo	 = &descriptorBufferInfo2;
		vkUpdateDescriptorSets(a_device, 1, &writeDescriptorSet2, 0, NULL);
	}
	
void ComputeApplication::createComputePipeline(VkDevice a_device, const VkDescriptorSetLayout& a_dsLayout,
						VkShaderModule* a_pShaderModule, VkPipeline* a_pPipeline,
						 VkPipelineLayout* a_pPipelineLayout)
	{
		
		
		
		
		std::vector<uint32_t> code = vk_utils::ReadFile("Shaders/comp.spv");
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pCode	= code.data();
		createInfo.codeSize = code.size()*sizeof(uint32_t);
	
		VK_CHECK_RESULT(vkCreateShaderModule(a_device, &createInfo, NULL, a_pShaderModule));
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
		shaderStageCreateInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage	= VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageCreateInfo.module = (*a_pShaderModule);
		shaderStageCreateInfo.pName	= "main";

		//// Allow pass (w,h) inside shader directly from command buffer
		//
		VkPushConstantRange pcRange = {};	// #NOTE: we updated this to pass W/H inside shader
		pcRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		pcRange.offset	 = 0;
		pcRange.size		 = num_push_constants*sizeof(int); // 3 ints

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType		= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts	= &a_dsLayout;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges	= &pcRange;
		VK_CHECK_RESULT(vkCreatePipelineLayout(a_device, &pipelineLayoutCreateInfo, NULL, a_pPipelineLayout));

		VkComputePipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType	= VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stage	= shaderStageCreateInfo;
		pipelineCreateInfo.layout = (*a_pPipelineLayout);
		
		
		VK_CHECK_RESULT(vkCreateComputePipelines(a_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, a_pPipeline));
	}
	
void ComputeApplication::createCommandBuffer(VkDevice a_device, uint32_t queueFamilyIndex, VkPipeline a_pipeline, VkPipelineLayout a_layout,
					VkCommandPool* a_pool, VkCommandBuffer* a_pCmdBuff)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = 0;
		
		
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
		VK_CHECK_RESULT(vkCreateCommandPool(a_device, &commandPoolCreateInfo, NULL, a_pool));
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType		 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = (*a_pool); 
		
		
		
		commandBufferAllocateInfo.level			= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1; 
		VK_CHECK_RESULT(vkAllocateCommandBuffers(a_device, &commandBufferAllocateInfo, a_pCmdBuff)); 
	}
	
void ComputeApplication::recordCommandsTo(VkCommandBuffer a_cmdBuff, VkPipeline a_pipeline, VkPipelineLayout a_layout, const VkDescriptorSet& a_ds)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; 
		VK_CHECK_RESULT(vkBeginCommandBuffer(a_cmdBuff, &beginInfo)); 
		vkCmdBindPipeline(a_cmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, a_pipeline);
		vkCmdBindDescriptorSets(a_cmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, a_layout, 0, 1, &a_ds, 0, NULL);

		// push constants
		int wh[num_push_constants] = {WIDTH, HEIGHT, mlp.num_hidden_layers, mlp.hidden_layer_size};
		vkCmdPushConstants(a_cmdBuff, a_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(int)*num_push_constants, wh);

		vkCmdDispatch(a_cmdBuff, (uint32_t)ceil(WIDTH / float(WORKGROUP_SIZE)), (uint32_t)ceil(HEIGHT / float(WORKGROUP_SIZE)), 1);
		VK_CHECK_RESULT(vkEndCommandBuffer(a_cmdBuff)); 
	}
	
void ComputeApplication::runCommandBuffer(VkCommandBuffer a_cmdBuff, VkQueue a_queue, VkDevice a_device)
{
	VkSubmitInfo submitInfo = {};
	submitInfo.sType			= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1; 
	submitInfo.pCommandBuffers	= &a_cmdBuff; 
	VkFence fence;
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;
	VK_CHECK_RESULT(vkCreateFence(a_device, &fenceCreateInfo, NULL, &fence));
	VK_CHECK_RESULT(vkQueueSubmit(a_queue, 1, &submitInfo, fence));
	VK_CHECK_RESULT(vkWaitForFences(a_device, 1, &fence, VK_TRUE, 100000000000));
	vkDestroyFence(a_device, fence, NULL);
}
void ComputeApplication::cleanup() {
	if (enableValidationLayers) {
		
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func == nullptr) {
		throw std::runtime_error("Could not load vkDestroyDebugReportCallbackEXT");
		}
		func(instance, debugReportCallback, NULL);
	}
	vkFreeMemory(device, bufferMemoryPixels, NULL);
	vkDestroyBuffer(device, bufferPixels, NULL);

	vkFreeMemory(device, bufferMemoryStaging, NULL);
	vkDestroyBuffer(device, bufferStaging, NULL);
	vkFreeMemory(device, bufferMemoryWeightsDevice, NULL);
	vkDestroyBuffer(device, bufferWeightsDevice, NULL);

	vkDestroyShaderModule(device, computeShaderModule, NULL);
	vkDestroyDescriptorPool(device, descriptorPool, NULL);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
	vkDestroyPipelineLayout(device, pipelineLayout, NULL);
	vkDestroyPipeline(device, pipeline, NULL);
	vkDestroyCommandPool(device, commandPool, NULL);	
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);	
	}
