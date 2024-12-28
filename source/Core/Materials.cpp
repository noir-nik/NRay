#ifdef USE_MODULES
module;
#endif

#include "GpuTypes.h"

#ifdef USE_MODULES
module Materials;
import VulkanBackend;
#else
#include "VulkanBackend.cppm"
#include "Materials.cppm"
#endif

void Materials::Init(vkw::Device device, vkw::Queue queue, uint capacity){
	this->device = device;
	this->queue = queue;
	bufferCapacity = capacity;
}

auto Materials::Expand() -> void {
	buffers.emplace_back(device.CreateBuffer({
		bufferCapacity * sizeof(GpuTypes::Material), vkw::BufferUsage::Storage | vkw::BufferUsage::TransferDst, vkw::Memory::GPU, "Materials[" + std::to_string(buffers.size()) + "]"
	}));
	auto size = availableSlots.size();
	availableSlots.resize(size + bufferCapacity);
	uint idx = bufferCapacity * buffers.size();
	for (uint i = 0; i < bufferCapacity; ++i) {
		availableSlots[size + i] = --idx;
	}
}

auto Materials::CreateSlot() -> uint {
	if (availableSlots.empty()) [[unlikely]] Expand();
	auto id = availableSlots.back();
	availableSlots.pop_back();
	return id;
}

auto Materials::FreeSlot(uint materialID) -> void {
	availableSlots.push_back(materialID);
}

auto Materials::GetBuffer(uint materialID) -> vkw::Buffer& {
	return buffers[materialID / bufferCapacity];
}

auto Materials::GetOffset(uint materialID) -> uint {
	return (materialID % bufferCapacity) * sizeof(GpuTypes::Material);
}