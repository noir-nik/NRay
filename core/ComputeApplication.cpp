#include "Pch.hpp"

#include <vector>
#include <string.h>
#include <assert.h>
#include <stdexcept>
#include <cmath>
#include <iostream>

#include "ShaderCommon.h"

#include "vk_utils.h"
#include "FileManager.hpp"
#include "ComputeApplication.hpp"


#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

const int WIDTH		= SCREEN_WIDTH;
const int HEIGHT	 = SCREEN_HEIGHT;
// const int WORKGROUP_SIZE = 16;	
constexpr int num_push_constants = 4;
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


static const char *VK_ERROR_STRING(VkResult result);

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
	const uint32_t stagingBufferSize = 32 * 1024 * 1024;

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

	void createDescriptorSetLayout();
	void createDescriptorSet();

	void createBindlessResources();
	void destroyBindlessResources();

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






	void saveRenderedImageFromDeviceMemory(VkDevice a_device, VkDeviceMemory a_bufferMemory, size_t a_offset, int a_width, int a_height);    

	void createBufferApp(VkDevice a_device, VkPhysicalDevice a_physDevice, 
					VkDeviceSize a_size, VkBufferUsageFlags a_usage, VkMemoryPropertyFlags a_properties,
					VkBuffer& a_buffer, VkDeviceMemory& a_bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkQueue queue);
	// pixels + second buffer
	void createDescriptorSetLayoutApp(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout);
	void createDescriptorSetFor_2_Buffers(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize,
												VkBuffer a_secondBuffer, size_t a_secondBufferSize,
												const VkDescriptorSetLayout* a_pDSLayout,
												VkDescriptorPool* a_pDSPool, VkDescriptorSet* a_pDS);
												
	void createComputePipeline(VkDevice a_device, const VkDescriptorSetLayout& a_dsLayout,
									VkShaderModule* a_pShaderModule, VkPipeline* a_pPipeline,
									VkPipelineLayout* a_pPipelineLayout);

	void createCommandBuffer(VkDevice a_device, uint32_t queueFamilyIndex, VkPipeline a_pipeline, VkPipelineLayout a_layout,
									VkCommandPool* a_pool, VkCommandBuffer* a_pCmdBuff);

	// void recordCommandsTo(VkCommandBuffer a_cmdBuff, VkPipeline a_pipeline, VkPipelineLayout a_layout, const VkDescriptorSet& a_ds);
	void runCommandBuffer(VkCommandBuffer a_cmdBuff, VkQueue a_queue, VkDevice a_device);
	void cleanup();

	void run();

	// VkInstance instance;
	VkDebugReportCallbackEXT debugReportCallback;
	// VkPhysicalDevice physicalDevice;
	// VkDevice device;
	VkPipeline       pipeline = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkShaderModule   computeShaderModule = VK_NULL_HANDLE;
	VkCommandPool   commandPool = VK_NULL_HANDLE;
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	VkDescriptorPool      descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet       descriptorSet = VK_NULL_HANDLE;
	// //std::vector<VkDescriptorSet> descriptorSets;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkBuffer       bufferPixelsApp,
					bufferStagingApp,
					bufferWeightsDeviceApp;
	VkDeviceMemory bufferMemoryPixels, bufferMemoryStaging, bufferMemoryWeightsDevice;
	std::vector<const char *> enabledLayers;
	// VkQueue queue = VK_NULL_HANDLE; 

	// Buffer bufferPixel;
	// Buffer bufferWeightsDevice;


	struct MLP{
	std::vector<float> weights;
	int num_hidden_layers;
	int hidden_layer_size;
	};
	MLP mlp;
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


void Init() {
	_ctx.CreateInstance();
	_ctx.CreatePhysicalDevice();
	_ctx.CreateDevice();

	// _ctx.CreateSurfaceFormats();
	// _ctx.CreateSwapChain(width, height);
	
	// _ctx.createCommandBuffers();

	// _ctx.CreateImGui(window);
}

void Destroy() {
	// ImGui_ImplVulkan_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// _ctx.DestroySwapChain();

	// _ctx.DestroyCommandBuffers();
	_ctx.DestroyDevice();
	_ctx.DestroyInstance();
}


void ComputeApplication::run()	{
	Init();
	_ctx.mlp.weights = weights;
	_ctx.mlp.num_hidden_layers = numLayers;
	_ctx.mlp.hidden_layer_size = layerSize;
	_ctx.run();
	_ctx.cleanup();
	Destroy();
}

// void C

void Context::run()	{
	const int deviceId = 0;
	std::cout << "init vulkan for device " << deviceId << " ... " << std::endl;

	// uint32_t queueFamilyIndex = vk_utils::GetComputeQueueFamilyIndex(physicalDevice);
	
	// device = vk_utils::CreateLogicalDevice(queues[Queue::Compute].family, physicalDevice, enabledLayers);

	// vkGetDeviceQueue(device, queues[Queue::Compute].family, 0, &queue);
	
	size_t bufferSize = sizeof(Pixel) * WIDTH * HEIGHT;
	std::cout << "creating resources ... " << std::endl;
	//pixels
	createBufferApp(device, physicalDevice, bufferSize,	
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,	
		bufferPixelsApp, bufferMemoryPixels);	 

	// bufferPixel = CreateBuffer(bufferSize, vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::CPU, "Output Image");

	// //Staging buffer
	size_t weightsBufferSize = mlp.weights.size() * sizeof(float);//////TODO		
	createBufferApp(device, physicalDevice, weightsBufferSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			bufferStagingApp, bufferMemoryStaging);

	

	// //Weights buffer on device
	createBufferApp(device, physicalDevice, weightsBufferSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			bufferWeightsDeviceApp, bufferMemoryWeightsDevice);

	// Buffer bufferWeightsDevice = CreateBuffer(weightsBufferSize, vkw::BufferUsage::Storage | vkw::BufferUsage::TransferSrc, vkw::Memory::GPU, "Weights Buffer");

	//copy weights
	void* data;
	vkMapMemory(device, bufferMemoryStaging, 0, weightsBufferSize, 0, &data);
	memcpy(data, mlp.weights.data(), (size_t)weightsBufferSize);
	vkUnmapMemory(device, bufferMemoryStaging);


	createDescriptorSetLayout();
	createDescriptorSet();


	// createDescriptorSetLayoutApp(device, &descriptorSetLayout);	
	// //pixels + weights	 
			
				
	// createDescriptorSetFor_2_Buffers(device, bufferPixelsApp, bufferSize,
	// 				bufferWeightsDeviceApp, weightsBufferSize,
	// 				&descriptorSetLayout, 
	// 				&descriptorPool, &descriptorSet);	


		 //pixels
	VkDescriptorBufferInfo descriptorBufferInfo = {};
	descriptorBufferInfo.buffer = bufferPixelsApp;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range	= bufferSize;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType		 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet		= descriptorSet; 
	writeDescriptorSet.dstBinding		= 0;	
	writeDescriptorSet.descriptorCount = 1;	
	writeDescriptorSet.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; 
	writeDescriptorSet.pBufferInfo	 = &descriptorBufferInfo;
	vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, NULL);

	//second buffer
	VkDescriptorBufferInfo descriptorBufferInfo2 = {};
	descriptorBufferInfo2.buffer = bufferWeightsDeviceApp;
	descriptorBufferInfo2.offset = 0;
	descriptorBufferInfo2.range	= weightsBufferSize;
	
	VkWriteDescriptorSet writeDescriptorSet2 = {};
	writeDescriptorSet2.sType		 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet2.dstSet		= descriptorSet; 
	writeDescriptorSet2.dstBinding		= 1;	
	writeDescriptorSet2.descriptorCount = 1;	
	writeDescriptorSet2.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; 
	writeDescriptorSet2.pBufferInfo	 = &descriptorBufferInfo2;
	vkUpdateDescriptorSets(device, 1, &writeDescriptorSet2, 0, NULL);


	std::cout << "compiling shaders	... " << std::endl;
	createComputePipeline(device, descriptorSetLayout,
			&computeShaderModule, &pipeline, &pipelineLayout);
	createCommandBuffer(device, queues[Queue::Compute].family, pipeline, pipelineLayout,
			&commandPool, &commandBuffer);
	// DO
	// copy to device
	copyBuffer(bufferStagingApp, bufferWeightsDeviceApp, weightsBufferSize, queues[Queue::Compute].queue);


	// recordCommandsTo(commandBuffer, pipeline, pipelineLayout, descriptorSet);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; 
	VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo)); 
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

	// CmdCopy(bufferWeightsDevice, mlp.weights.data(), (size_t)weightsBufferSize, 0);

	// push constants
	int wh[num_push_constants] = {WIDTH, HEIGHT, mlp.num_hidden_layers, mlp.hidden_layer_size};
	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(int)*num_push_constants, wh);

	vkCmdDispatch(commandBuffer, (uint32_t)ceil(WIDTH / float(WORKGROUP_SIZE)), (uint32_t)ceil(HEIGHT / float(WORKGROUP_SIZE)), 1);
	VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer)); 

	std::cout << "doing computations ... " << std::endl;
	runCommandBuffer(commandBuffer, queues[Queue::Compute].queue, device);
	
	
	std::cout << "saving image		 ... " << std::endl;
	saveRenderedImageFromDeviceMemory(device, bufferMemoryPixels, 0, WIDTH, HEIGHT);
	
	std::cout << "destroying all	 ... " << std::endl;
	
}


	
void Context::saveRenderedImageFromDeviceMemory(VkDevice a_device, VkDeviceMemory a_bufferMemory, size_t a_offset, int a_width, int a_height)
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

void Context::createBufferApp(VkDevice a_device, VkPhysicalDevice a_physDevice, 
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

void Context::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkQueue queue) {
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
void Context::createDescriptorSetLayoutApp(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout)
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
	
void Context::createDescriptorSetFor_2_Buffers(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize,
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
	
void Context::createComputePipeline(VkDevice a_device, const VkDescriptorSetLayout& a_dsLayout,
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
	
void Context::createCommandBuffer(VkDevice a_device, uint32_t queueFamilyIndex, VkPipeline a_pipeline, VkPipelineLayout a_layout,
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
	
// void Context::recordCommandsTo(VkCommandBuffer a_cmdBuff, VkPipeline a_pipeline, VkPipelineLayout a_layout, const VkDescriptorSet& a_ds)
// 	{
// 		VkCommandBufferBeginInfo beginInfo = {};
// 		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
// 		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; 
// 		VK_CHECK_RESULT(vkBeginCommandBuffer(a_cmdBuff, &beginInfo)); 
// 		vkCmdBindPipeline(a_cmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, a_pipeline);
// 		vkCmdBindDescriptorSets(a_cmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, a_layout, 0, 1, &a_ds, 0, NULL);

// 		// push constants
// 		int wh[num_push_constants] = {WIDTH, HEIGHT, mlp.num_hidden_layers, mlp.hidden_layer_size};
// 		vkCmdPushConstants(a_cmdBuff, a_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(int)*num_push_constants, wh);

// 		vkCmdDispatch(a_cmdBuff, (uint32_t)ceil(WIDTH / float(WORKGROUP_SIZE)), (uint32_t)ceil(HEIGHT / float(WORKGROUP_SIZE)), 1);
// 		VK_CHECK_RESULT(vkEndCommandBuffer(a_cmdBuff)); 
// 	}
	
void Context::runCommandBuffer(VkCommandBuffer a_cmdBuff, VkQueue a_queue, VkDevice a_device)
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
void Context::cleanup() {
	// if (enableValidationLayers) {
		
	// 	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	// 	if (func == nullptr) {
	// 	throw std::runtime_error("Could not load vkDestroyDebugReportCallbackEXT");
	// 	}
	// 	func(instance, debugReportCallback, NULL);
	// }
	vkFreeMemory(device, bufferMemoryPixels, NULL);
	vkDestroyBuffer(device, bufferPixelsApp, NULL);

	vkFreeMemory(device, bufferMemoryStaging, NULL);
	vkDestroyBuffer(device, bufferStagingApp, NULL);
	vkFreeMemory(device, bufferMemoryWeightsDevice, NULL);
	vkDestroyBuffer(device, bufferWeightsDeviceApp, NULL);

	vkDestroyShaderModule(device, computeShaderModule, NULL);
	vkDestroyDescriptorPool(device, descriptorPool, NULL);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
	vkDestroyPipelineLayout(device, pipelineLayout, NULL);
	vkDestroyPipeline(device, pipeline, NULL);
	vkDestroyCommandPool(device, commandPool, NULL);	
	// vkDestroyDevice(device, NULL);
	// vkDestroyInstance(instance, NULL);	
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

	static int bufferRID = 0;
	if (usage & BufferUsage::Storage) {
		// res->rid = _ctx.availableBufferRID.back(); // TODO test: give RID starting from 0, not from end
		// _ctx.availableBufferRID.pop_back();
		res->rid = bufferRID;

		VkDescriptorBufferInfo descriptorInfo = {};
		descriptorInfo.buffer = res->buffer;
		descriptorInfo.offset = 0;
		descriptorInfo.range  = size;

		VkWriteDescriptorSet write = {};
		write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		// write.dstSet          = _ctx.bindlessDescriptorSet;
		write.dstSet          = _ctx.descriptorSet;
		// write.dstBinding      = BINDING_BUFFER;
		write.dstBinding      = bufferRID;
		// write.dstArrayElement = buffer.RID();
		write.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo     = &descriptorInfo;
		vkUpdateDescriptorSets(_ctx.device, 1, &write, 0, nullptr);
		bufferRID++;
	}

	return buffer;
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

void Context::createDescriptorSetLayout(){
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

	VkResult result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, NULL, &descriptorSetLayout);
	DEBUG_VK(result, "Failed to allocate bindless descriptor set!");
	ASSERT(result == VK_SUCCESS, "Failed to allocate bindless descriptor set!");
}

// vkCreateDescriptorPool + vkAllocateDescriptorSets
void Context::createDescriptorSet()
{
	VkDescriptorPoolSize descriptorPoolSize[2];
	descriptorPoolSize[0].type		      = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPoolSize[0].descriptorCount = 1;
	descriptorPoolSize[1].type		      = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPoolSize[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType	       = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.maxSets	   = 1; 
	descriptorPoolCreateInfo.poolSizeCount = 2;
	descriptorPoolCreateInfo.pPoolSizes    = descriptorPoolSize;
	VkResult result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, NULL, &descriptorPool);

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType			     = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool	 = descriptorPool; 
	descriptorSetAllocateInfo.descriptorSetCount = 1;		
	descriptorSetAllocateInfo.pSetLayouts	     = &descriptorSetLayout;
	result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);
	
}	
		

void Context::createBindlessResources(){
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
    CmdCopy(dst, cmd.staging, size, cmd.stagingOffset);
    cmd.stagingOffset += size;
}

void Context::CmdCopy(Image& dst, Buffer& src, uint32_t size, uint32_t srcOffset) {
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

void Context::CmdBarrier(Image& img, Layout::ImageLayout layout) {
    CommandResources& cmd = GetCurrentCommandResources();
    VkImageSubresourceRange range = {};
    range.aspectMask = (VkImageAspectFlags)img.aspect;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;;

    VkAccessFlags srcAccess = VK_ACCESS_SHADER_WRITE_BIT;
    VkAccessFlags dstAccess = VK_ACCESS_SHADER_READ_BIT;
    VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    barrier.oldLayout = (VkImageLayout)img.layout;
    barrier.newLayout = (VkImageLayout)layout;
    barrier.image = img.resource->image;
    barrier.subresourceRange = range;
    vkCmdPipelineBarrier(cmd.buffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    img.layout = layout;
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
