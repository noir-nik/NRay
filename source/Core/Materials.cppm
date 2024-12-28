#ifdef USE_MODULES
export module Materials;
#define _MATERIALS_EXPORT export
import Types;
import vulkan_backend;
import std;
#else
#pragma once
#define _MATERIALS_EXPORT
#include <vector>

#include "Types.cppm"
#include "vulkan_backend.hpp"
#endif

static constexpr uint defaultCapacity = 256;

/**
 * \brief Materials class
 * \details This class is a material system, with simple interface for storing materials.
 *          It will create gpu buffers and keep track of available slots for materials.
 */
_MATERIALS_EXPORT
class Materials {
public:
	void Init(vb::Device device, vb::Queue queue, uint capacity = defaultCapacity);
	[[nodiscard]] auto GetBuffer(uint slot) -> vb::Buffer&;
	[[nodiscard]] auto GetOffset(uint slot) -> uint;
	[[nodiscard]] auto CreateSlot() -> uint;
	auto FreeSlot (uint slot) -> void;
private:
	vb::Device device;
	vb::Queue queue;
	std::vector<vb::Buffer> buffers;
	std::vector<uint> available_slots;
	uint buffer_capacity;
	void Expand();
};

#undef _MATERIALS_EXPORT