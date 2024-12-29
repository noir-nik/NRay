#ifdef USE_MODULES
module;
#else
#pragma once
#endif

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

// NOLINTBEGIN(misc-unused-using-decls)
#ifdef USE_MODULES
export module vk_mem_alloc;

export using ::VmaAllocation;
export using ::VmaAllocator;
export using ::VmaMemoryUsage;
export using ::VmaVulkanFunctions;

export using ::VmaAllocatorCreateInfo;
export using ::VmaAllocatorCreateFlags;

export using ::VmaAllocationCreateInfo;
export using ::VmaAllocationCreateFlags;

export using ::vmaDestroyAllocator;
export using ::vmaCreateAllocator;

export using ::vmaMapMemory;
export using ::vmaUnmapMemory;

export using ::vmaCreateBuffer;
export using ::vmaDestroyBuffer;

export using ::vmaCreateImage;
export using ::vmaDestroyImage;

export using ::vmaCopyMemoryToAllocation;

#endif
// NOLINTEND(misc-unused-using-decls)