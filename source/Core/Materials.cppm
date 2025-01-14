export module Materials;
import Types;
import vulkan_backend;
import std;


/**
 * \brief Materials class
 * \details This class is a material system, with simple interface for storing materials.
 *          It will create gpu buffers and keep track of available slots for materials.
 */
export
class Materials {
public:
	uint static constexpr defaultCapacity = 256;
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