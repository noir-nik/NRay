module;

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

// NOLINTBEGIN(misc-unused-using-decls)
export module vk_mem_alloc;
export {
using ::VmaAllocation;
using ::VmaAllocator;
using ::VmaMemoryUsage;
using ::VmaVulkanFunctions;
using ::VmaAllocatorCreateInfo;
using ::VmaAllocatorCreateFlags;
using ::VmaAllocationCreateInfo;
using ::VmaAllocationCreateFlags;
using ::vmaDestroyAllocator;
using ::vmaCreateAllocator;
using ::vmaMapMemory;
using ::vmaUnmapMemory;
using ::vmaCreateBuffer;
using ::vmaDestroyBuffer;
using ::vmaCreateImage;
using ::vmaDestroyImage;
using ::vmaCopyMemoryToAllocation;
}
// NOLINTEND(misc-unused-using-decls)