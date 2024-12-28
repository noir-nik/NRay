#ifdef USE_MODULES
export module Materials;
#define _MATERIALS_EXPORT export
import Types;
import VulkanBackend;
import stl;
#else
#pragma once
#define _MATERIALS_EXPORT
#include "Types.cppm"
#include "VulkanBackend.cppm"
#endif

static constexpr uint defaultCapacity = 256;

/**
 * \brief Materials class
 * \details This class is a material system, with simple interface for storing materials.
 *          It will create gpu buffers and keep track of available slots for materials.
 */
_MATERIALS_EXPORT
struct Materials {
	void Init(vkw::Device device, vkw::Queue queue, uint capacity = defaultCapacity);
	[[nodiscard]] auto GetBuffer(uint slot) -> vkw::Buffer&;
	[[nodiscard]] auto GetOffset(uint slot) -> uint;
	[[nodiscard]] auto CreateSlot() -> uint;
	auto FreeSlot (uint slot) -> void;
private:
	vkw::Device device;
	vkw::Queue queue;
	std::vector<vkw::Buffer> buffers;
	std::vector<uint> availableSlots;
	uint bufferCapacity;
	void Expand();
};

#undef _MATERIALS_EXPORT