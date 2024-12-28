#pragma once
#include <cstdint>
#include <imgui/imgui.h>
#include <span>

#include <vector>
#include <filesystem>
#include <memory>
#include <string>

#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.hpp"
#endif

struct GLFWwindow;

namespace vkw {

using float4 = Lmath::float4;

using vec4  = Lmath::vec4;
using ivec4 = Lmath::ivec4;
using ivec2 = Lmath::ivec2;
using uvec3 = Lmath::uvec3;
using uvec2 = Lmath::uvec2;

using Flags = uint32_t;
using Flags64 = uint64_t;

using DeviceAddress = uint64_t;
using DeviceSize = uint64_t;

#define QueueFamilyIgnored (~0U)
#define WholeSize          (~0ULL)

enum Memory {
	GPU = 0x00000001,
	CPU = 0x0000000 | 0x00000004,
};
using MemoryFlags = Flags;

namespace BufferUsage {
	enum {
		TransferSrc                = 0x00000001,
		TransferDst                = 0x00000002,
		UniformTexel               = 0x00000004,
		StorageTexel               = 0x00000008,
		Uniform                    = 0x00000010,
		Storage                    = 0x00000020,
		Index                      = 0x00000040,
		Vertex                     = 0x00000080,
		Indirect                   = 0x00000100,
		Address                    = 0x00020000,
		VideoDecodeSrc             = 0x00002000,
		VideoDecodeDst             = 0x00004000,
		TransformFeedback          = 0x00000800,
		TransformFeedbackCounter   = 0x00001000,
		ConditionalRendering       = 0x00000200,
		AccelerationStructureInput = 0x00080000,
		AccelerationStructure      = 0x00100000,
		ShaderBindingTable         = 0x00000400,
		SamplerDescriptor          = 0x00200000,
		ResourceDescriptor         = 0x00400000,
		PushDescriptors            = 0x04000000,
		MicromapBuildInputReadOnly = 0x00800000,
		MicromapStorage            = 0x01000000,
	};
}
using BufferUsageFlags = Flags;

enum Format {
	Undefined         = 0,

	R8_unorm          = 9,  // Sampled, heightmap

	RGBA8_UNORM       = 37, // ColorAttachment
	RGBA8_SRGB        = 43,
	BGRA8_unorm       = 44,
	BGRA8_sRGB        = 50,
	RGBA16_sfloat     = 97,

	RG32_sfloat       = 103, // VertexAttribute
	RGB32_sfloat      = 106, 
	RGBA32_sfloat     = 109, 

	D16_unorm         = 124, // DepthStencilAttachment
	D32_sfloat        = 126, // DepthAttachment
	D24_unorm_S8_uint = 129, // DepthStencilAttachment
};

namespace ImageUsage {
	enum {
		TransferSrc            = 0x00000001,
		TransferDst            = 0x00000002,
		Sampled                = 0x00000004,
		Storage                = 0x00000008,
		ColorAttachment        = 0x00000010,
		DepthStencilAttachment = 0x00000020,
		TransientAttachment    = 0x00000040,
		InputAttachment        = 0x00000080,
	};
}
using ImageUsageFlags = Flags;

namespace Aspect {
	enum {
		Color = 1,
		Depth = 2,
		Stencil = 4,
	};
}
using AspectFlags = Flags;

enum class ImageLayout {
// VK_IMAGE_LAYOUT_
	Undefined                      = 0,
	General                        = 1,
	ColorAttachment                = 2,
	DepthStencilAttachment         = 3,
	DepthStencilRead               = 4,
	ShaderRead                     = 5,
	TransferSrc                    = 6,
	TransferDst                    = 7,
	Preinitialized                 = 8,
	DepthReadOnlyStencilAttachment = 1000117000,
	DepthAttachmentStencilRead     = 1000117001,
	DepthAttachment                = 1000241000,
	DepthReadOnly                  = 1000241001,
	StencilAttachment              = 1000241002,
	StencilReadOnly                = 1000241003,
	ReadOnly                       = 1000314000,
	Attachment                     = 1000314001,
	Present                        = 1000001002,

	MaxEnum = 0x7FFFFFFF
};


enum class SampleCount {
// namespace SampleCount {
	// enum {
		_1 = 0x00000001,
		_2 = 0x00000002,
		_4 = 0x00000004,
		_8 = 0x00000008,
		_16 = 0x00000010,
		_32 = 0x00000020,
		_64 = 0x00000040,
		MaxEnum = 0x7FFFFFFF
	// };
};
// using SampleCountFlags = Flags;

struct DeviceResource;
struct SwapChainResource;
struct BufferResource;
struct ImageResource;
struct PipelineResource;
struct CommandResource;
struct QueueResource;
struct Semaphore;
struct Command;

struct Device;

struct Buffer {
	std::shared_ptr<BufferResource> resource;
	uint64_t size;
	BufferUsageFlags usage;
	MemoryFlags memory;
	uint32_t RID();
};

struct BufferDesc {
	uint64_t size;
	BufferUsageFlags usage;
	MemoryFlags memory = Memory::GPU;
	std::string name = "";
};

struct Image {
	std::shared_ptr<ImageResource> resource;
	uint32_t width = 0;
	uint32_t height = 0;
	ImageUsageFlags usage;
	Format format;
	ImageLayout layout;
	AspectFlags aspect;
	uint32_t layers = 1;
	uint32_t RID();
	// ImTextureID ImGuiRID();
	// ImTextureID ImGuiRID(uint32_t layer);
};

enum class Filter {
	Nearest = 0,
	Linear = 1,
	Cubic_Ext = 1000015000,
};

enum class MipmapMode {
	Nearest = 0,
	Linear = 1,
};

enum class WrapMode : std::uint16_t {
	Repeat = 0,
    MirroredRepeat = 1,
    ClampToEdge = 2,
    ClampToBorder = 3,
    MirrorClampToEdge = 4,
};

struct SamplerDesc {
	Filter magFilter = Filter::Linear;
	Filter minFilter = Filter::Linear;
	MipmapMode mipmapMode = MipmapMode::Linear;
	WrapMode wrapU = WrapMode::Repeat;
	WrapMode wrapV = WrapMode::Repeat;
	WrapMode wrapW = WrapMode::Repeat;
	float maxAnisotropy = 0.0f;
	float minLod = 0.0f;
	float maxLod = 1.0f;
};

struct ImageDesc {
	uint32_t width;
	uint32_t height;
	Format format;
	ImageUsageFlags usage;
	SampleCount samples = SampleCount::_1;
	SamplerDesc sampler;
	std::string name = "";
	uint32_t layers = 1;
};

namespace QueueFlagBits {
	enum {
		Graphics = 0x00000001,
		Compute = 0x00000002,
		Transfer = 0x00000004,
		SparseBinding = 0x00000008, 
		Protected = 0x00000010,
		VideoDecode = 0x00000020,
		VideoEncode = 0x00000040,
		OpticalFlow = 0x00000100,
		MaxEnum = 0x7FFFFFFF
	};
}
using QueueFlags = Flags;

struct Queue {
	QueueFlags flags = 0;
	GLFWwindow* supportedWindowToPresent = {};
	bool preferSeparateFamily = false;
	Queue() = default;
	Queue(QueueFlags flags, GLFWwindow* supportedWindowToPresent = {}, bool preferSeparateFamily = false):
		flags(flags),
		supportedWindowToPresent(supportedWindowToPresent),
		preferSeparateFamily(preferSeparateFamily) {};

	Command& GetCommandBuffer();
	
	operator bool() const {
		return resource != nullptr;
	}

private:
	friend Device CreateDevice(const std::vector<Queue*>& queues);
	friend Device;
	friend class SwapChain;
	friend class DeviceResource;
	friend class CommandResource;
	friend class SwapChainResource;
	std::shared_ptr<QueueResource> resource;
	Queue(const std::shared_ptr<QueueResource>& resource);
};


// enum class Queue {
// 	Graphics = 0,
// 	Compute = 1,
// 	Transfer = 2,
// 	Count = 3,
// };

enum class PipelinePoint {
	Graphics = 0,
	Compute = 1,
	RayTracing = 1000165000,
};

enum class ShaderStage {
	Vertex = 0x00000001,
	TessellationControl = 0x00000002,
	TessellationEvaluation = 0x00000004,
	Geometry = 0x00000008,
	Fragment = 0x00000010,
	Compute = 0x00000020,
	AllGraphics = 0x0000001F,
	All = 0x7FFFFFFF,

	Raygen = 0x00000100,
	AnyHit = 0x00000200,
	ClosestHit = 0x00000400,
	Miss = 0x00000800,
	Intersection = 0x00001000,
	Callable = 0x00002000,

	Task = 0x00000040,
	Mesh = 0x00000080,
	MaxEnum = 0x7FFFFFFF
};

struct Pipeline {
	struct Stage {
		ShaderStage stage;
		std::filesystem::path path;
		std::string entryPoint = "main";
	private:
	friend class DeviceResource;
		size_t Hash() const;
	};
	PipelinePoint point;
	std::shared_ptr<PipelineResource> resource;
	// std::vector<Stage> stages;
	// std::vector<std::vector<char>> stageBytes;
};

namespace CullMode {
	enum {
		None         = 0,
		Front        = 1,
		Back         = 2,
		FrontAndBack = 3,
		MaxEnum      = 0x7FFFFFFF
	};
}
using CullModeFlags = Flags;

enum class VertexAttributeLayout {
	InterLeaved = 0,
	Separate = 1,
};

struct PipelineDesc {
	PipelinePoint point;
	std::vector<Pipeline::Stage> stages;
	std::string name = "";
	std::vector<Format> vertexAttributes;
	VertexAttributeLayout vertexAttributeLayout = VertexAttributeLayout::InterLeaved;
	std::vector<Format> colorFormats;
	bool useDepth = false;
	Format depthFormat;
	SampleCount samples = vkw::SampleCount::_1;
	CullModeFlags cullMode = CullMode::None;
	bool lineTopology = false;
};


/* ===== Synchronization 2 Stages ===== */
namespace PipelineStage {
	enum : Flags64 {
		None                         = 0ULL,
		TopOfPipe                    = 0x00000001ULL, // == None as src | AllCommands with AccessFlags set to 0 as dst
		DrawIndirect                 = 0x00000002ULL,
		VertexInput                  = 0x00000004ULL,
		VertexShader                 = 0x00000008ULL,
		TessellationControlShader    = 0x00000010ULL,
		TessellationEvaluationShader = 0x00000020ULL,
		GeometryShader               = 0x00000040ULL,
		FragmentShader               = 0x00000080ULL,
		EarlyFragmentTests           = 0x00000100ULL,
		LateFragmentTests            = 0x00000200ULL,
		ColorAttachmentOutput        = 0x00000400ULL,
		ComputeShader                = 0x00000800ULL,
		AllTransfer                  = 0x00001000ULL,
		Transfer                     = 0x00001000ULL,
		BottomOfPipe                 = 0x00002000ULL, // == None as dst | AllCommands with AccessFlags set to 0 as src
		Host                         = 0x00004000ULL,
		AllGraphics                  = 0x00008000ULL,
		AllCommands                  = 0x00010000ULL,
		Copy                         = 0x100000000ULL,
		Resolve                      = 0x200000000ULL,
		Blit                         = 0x400000000ULL,
		Clear                        = 0x800000000ULL,
		IndexInput                   = 0x1000000000ULL,
		VertexAttributeInput         = 0x2000000000ULL,
		PreRasterizationShaders      = 0x4000000000ULL
	};
}
using PipelineStageFlags = Flags64;


/* ===== Synchronization 2 Access ===== */
namespace Access {
	enum : Flags64 {
		None                        = 0ULL,
		IndirectCommandRead         = 0x00000001ULL,
		IndexRead                   = 0x00000002ULL,
		VertexAttributeRead         = 0x00000004ULL,
		UniformRead                 = 0x00000008ULL,
		InputAttachmentRead         = 0x00000010ULL,
		ShaderRead                  = 0x00000020ULL,
		ShaderWrite                 = 0x00000040ULL,
		ColorAttachmentRead         = 0x00000080ULL,
		ColorAttachmentWrite        = 0x00000100ULL,
		DepthStencilAttachmentRead  = 0x00000200ULL,
		DepthStencilAttachmentWrite = 0x00000400ULL,
		TransferRead                = 0x00000800ULL,
		TransferWrite               = 0x00001000ULL,
		HostRead                    = 0x00002000ULL,
		HostWrite                   = 0x00004000ULL,
		MemoryRead                  = 0x00008000ULL,
		MemoryWrite                 = 0x00010000ULL,
		ShaderSampledRead           = 0x100000000ULL,
		ShaderStorageRead           = 0x200000000ULL,
		ShaderStorageWrite          = 0x400000000ULL,
	};
}
using AccessFlags = Flags64;


/* ===== Synchronization 2 Barriers ===== */
struct MemoryBarrier {
	PipelineStageFlags    srcStageMask  = PipelineStage::AllCommands;
	AccessFlags           srcAccessMask = Access::ShaderWrite;
	PipelineStageFlags    dstStageMask  = PipelineStage::AllCommands;
	AccessFlags           dstAccessMask = Access::ShaderRead;
};

struct BufferBarrier/* : MemoryBarrier */ {
	uint32_t      srcQueueFamilyIndex = QueueFamilyIgnored;
	uint32_t      dstQueueFamilyIndex = QueueFamilyIgnored;
	DeviceSize    offset              = 0;
	DeviceSize    size                = WholeSize;
	MemoryBarrier memoryBarrier;
};

struct ImageBarrier/* : MemoryBarrier */ {
	ImageLayout   newLayout           = ImageLayout::MaxEnum;
	ImageLayout   oldLayout           = ImageLayout::MaxEnum;
	uint32_t      srcQueueFamilyIndex = QueueFamilyIgnored;
	uint32_t      dstQueueFamilyIndex = QueueFamilyIgnored;
	MemoryBarrier memoryBarrier;
};

struct SubmitInfo {
	Semaphore* waitSemaphore   = nullptr;
	Flags64    waitStages      = PipelineStage::None;
	Semaphore* signalSemaphore = nullptr;
	Flags64    signalStages    = PipelineStage::None;
};


struct Command {
	std::shared_ptr<CommandResource> resource;
	Command();

	bool Copy(Buffer& dst, void*   data, uint32_t size, uint32_t dstOffset = 0);
	void Copy(Buffer& dst, Buffer& src,  uint32_t size, uint32_t dstOffset = 0, uint32_t srcOffset = 0);
	bool Copy(Image&  dst, void*   data, uint32_t size);
	void Copy(Image&  dst, Buffer& src,  uint32_t srcOffset = 0); // size is a No OP
	void Copy(Buffer& dst, Image& src, uint32_t dstOffset, ivec2 imageOffset, ivec2 imageExtent);// size is a No OP
	void Barrier(Image& img, const ImageBarrier& barrier);
	void Barrier(Buffer& buf, const BufferBarrier& barrier);
	void Barrier(const MemoryBarrier& barrier);
	void Blit(Image& dst, Image& src, ivec4 dstRegion = {}, ivec4 srcRegion = {}, Filter filter = Filter::Linear);
	void ClearColorImage(Image& image, const float4& color);


	void BeginRendering(const std::vector<std::span<const Image>>& colorAttachs, Image depthAttach = {}, uint32_t layerCount = 1, vec4 viewport = {}, ivec4 scissor = {});
	void EndRendering();
	// void BeginPresent();
	// void EndPresent();
	void BindPipeline(Pipeline& pipeline);
	void PushConstants(void* data, uint32_t size);

	// void BuildBLAS(BLAS& blas);
	// void BuildTLAS(TLAS& tlas, const std::vector<BLASInstance>& instances);

	void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	void BindVertexBuffer(Buffer& vertexBuffer);
	void BindIndexBuffer(Buffer& indexBuffer);
	void DrawMesh(Buffer& vertexBuffer, Buffer& indexBuffer, uint32_t indexCount);
	void DrawLineStrip(const Buffer& pointsBuffer, uint32_t firstPoint, uint32_t pointCount, float thickness = 1.0f);
	void DrawPassThrough();
	void DrawImGui(ImDrawData* data);
	void Dispatch(const uvec3& groups);

	int BeginTimeStamp(const std::string& name);
	void EndTimeStamp(int timeStampIndex);
	void BeginCommandBuffer();
	void EndCommandBuffer();
	void WaitQueue ();
	void QueueSubmit (const SubmitInfo& submitInfo);
};

struct Device {
	std::shared_ptr<DeviceResource> resource;
	Buffer CreateBuffer(const BufferDesc& desc);
	Image CreateImage(const ImageDesc& desc);
	Pipeline CreatePipeline(const PipelineDesc& desc);

	Queue GetQueue(QueueFlags flags);
	Command& GetCommandBuffer(const Queue& queue);
	void ResetStaging();
	
	void WaitQueue(Queue* queue);
	void WaitIdle();


	void* MapBuffer(Buffer& buffer);
	void UnmapBuffer(Buffer& buffer);
};


class SwapChain {
	std::shared_ptr<SwapChainResource> resource;

	std::vector<Image> swapChainImages;

	GLFWwindow* window = nullptr;	
	uint32_t framesInFlight = 2;
	uint32_t additionalImages = 0;
	uint32_t currentFrame = 0;
	bool dirty = true;
	uint32_t currentImageIndex = 0;

	// SwapChain() = default;

public:
	// ~SwapChain() { printf("~SwapChain\n"); }

	inline Image&      GetCurrentImage()   { return swapChainImages[currentImageIndex]; }
	Command&    GetCommandBuffer();

	void Create(Device& device, vkw::Queue& queue, GLFWwindow* window, uint32_t width, uint32_t height);
	void CreateUI(SampleCount sampleCount);
	void Destroy();
	void Recreate(uint32_t width, uint32_t height);
	bool AcquireImage();
	bool GetDirty();
	void SubmitAndPresent();
};



// TLAS CreateTLAS(uint32_t maxInstances, const std::string& name);
// BLAS CreateBLAS(const BLASDesc& desc);

void* MapBuffer(Buffer& buffer);
void UnmapBuffer(Buffer& buffer);

// void GetTimeStamps(std::map<std::string, float>& timeTable);



void Init(bool requestPresent = true);
Device CreateDevice(const std::vector<Queue*>& queues);
void Destroy();

void ImGuiNewFrame();
void ImGuiShutdown();

// template<typename T>
// void TimeStamp(const std::string& name, T callback) {
//     int id = BeginTimeStamp(name);
//     callback();
//     EndTimeStamp(id);
// }

}