module;

#include "GpuTypes.h"

module Materials;
import vulkan_backend;
void Materials::Init(vb::Device device, vb::Queue queue, uint capacity){
	this->device = device;
	this->queue = queue;
	buffer_capacity = capacity;
}

auto Materials::Expand() -> void {
	buffers.emplace_back(device.CreateBuffer({
		buffer_capacity * sizeof(GpuTypes::Material),
			vb::BufferUsage::eStorageBuffer | vb::BufferUsage::eTransferDst,
			vb::Memory::GPU, "Materials[" + std::to_string(buffers.size()) + "]"
	}));
	auto size = available_slots.size();
	available_slots.resize(size + buffer_capacity);
	uint idx = buffer_capacity * buffers.size();
	for (uint i = 0; i < buffer_capacity; ++i) {
		available_slots[size + i] = --idx;
	}
}

auto Materials::CreateSlot() -> uint {
	if (available_slots.empty()) [[unlikely]] Expand();
	auto id = available_slots.back();
	available_slots.pop_back();
	return id;
}

auto Materials::FreeSlot(uint materialID) -> void {
	available_slots.push_back(materialID);
}

auto Materials::GetBuffer(uint materialID) -> vb::Buffer& {
	return buffers[materialID / buffer_capacity];
}

auto Materials::GetOffset(uint materialID) -> uint {
	return (materialID % buffer_capacity) * sizeof(GpuTypes::Material);
}