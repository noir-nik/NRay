// #ifdef USE_MODULES
// module;
// #endif

// #include "Bindless.h"

#ifdef USE_MODULES
export module VulkanBackend;
import Lmath;
import stl;
import imgui; 
import Types;
#define _VKW_EXPORT export
#else
#pragma once
#define _VKW_EXPORT
#include "Lmath_types.h"
#include "Types.cppm"

#include <cstdint>
#include <span>

#include <vector>
#include <filesystem>
#include <memory>
#include <string>
#endif


_VKW_EXPORT
namespace vkw {
using namespace Lmath; 

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

	R8_UNORM          = 9,  // Sampled, heightmap

	RGBA8_UNORM       = 37, // ColorAttachment
	RGBA8_SRGB        = 43,
	BGRA8_UNORM       = 44,
	BGRA8_SRGB        = 50,
	RGBA16_SFLOAT     = 97,

	RG32_SFLOAT       = 103, // VertexAttribute
	RGB32_SFLOAT      = 106, 
	RGBA32_SFLOAT     = 109, 

	D16_UNORM         = 124, // DepthStencilAttachment
	D32_SFLOAT        = 126, // DepthAttachment
	D24_UNORM_S8_UINT = 129, // DepthStencilAttachment
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

struct Extent3D {
	Extent3D () : width(0), height(0), depth(1) {}
	Extent3D (uint32_t width, uint32_t height, uint32_t depth = 1) : width(width), height(height), depth(depth) {}
	Extent3D (uvec2 const size) : width(size.x), height(size.y), depth(1) {}
	Extent3D (uvec3 const size) : width(size.x), height(size.y), depth(size.z) {}
	uint32_t width;
	uint32_t height;
	uint32_t depth;
};

struct Image {
	std::shared_ptr<ImageResource> resource;
	Extent3D extent{};
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

enum class CompareOp {
	Never = 0,
	Less = 1,
	Equal = 2,
	LessOrEqual = 3,
	Greater = 4,
	NotEqual = 5,
	GreaterOrEqual = 6,
	Always = 7,
	MaxEnum = 0x7FFFFFFF
};

enum class BorderColor {
	IntOpaqueBlack = 0,
	IntOpaqueWhite = 1,
	FloatOpaqueBlack = 2,
	FloatOpaqueWhite = 3,
	MaxEnum = 0x7FFFFFFF
};

struct SamplerDesc {
	Filter magFilter = Filter::Linear;
	Filter minFilter = Filter::Linear;
	MipmapMode mipmapMode = MipmapMode::Linear;
	WrapMode wrapU = WrapMode::Repeat;
	WrapMode wrapV = WrapMode::Repeat;
	WrapMode wrapW = WrapMode::Repeat;
	float mipLodBias = 0.0f;
	bool anisotropyEnable = false;
	float maxAnisotropy = 0.0f;
	bool compareEnable = false;
	CompareOp compareOp = CompareOp::Always;
	float minLod = 0.0f;
	float maxLod = 1.0f;
	BorderColor borderColor = BorderColor::IntOpaqueBlack;
	bool unnormalizedCoordinates = false;
};

struct ImageDesc {
	Extent3D extent;
	Format format;
	ImageUsageFlags usage;
	SampleCount samples = SampleCount::_1;
	SamplerDesc sampler = {};
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
private:
	std::shared_ptr<QueueResource> resource;
public:
	QueueFlags flags = 0;
	void* supportedWindowToPresent = {};
	bool preferSeparateFamily = false;
	inline Queue() = default;
	inline Queue(QueueFlags flags, void* supportedWindowToPresent = {}, bool preferSeparateFamily = false):
		flags(flags),
		supportedWindowToPresent(supportedWindowToPresent),
		preferSeparateFamily(preferSeparateFamily) {};

	Command& GetCommandBuffer();
	
	inline operator bool() const {
		return resource != nullptr;
	}

private:
	friend Device CreateDevice(const std::span<Queue*> queues);
	friend Device;
	friend class SwapChain;
	friend DeviceResource;
	friend CommandResource;
	friend SwapChainResource;
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
		friend DeviceResource;
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

enum class DynamicState {
	ViewPort                    = 0,
	Scissor                     = 1,
	Line_Width                  = 2,
	Depth_Bias                  = 3,
	Blend_Constants             = 4,
	Depth_Bounds                = 5,
	Stencil_Compare_Mask        = 6,
	Stencil_Write_Mask          = 7,
	Stencil_Reference           = 8,
	Cull_Mode                   = 1000267000,
	Front_Face                  = 1000267001,
	Primitive_Topology          = 1000267002,
	ViewPort_With_Count         = 1000267003,
	Scissor_With_Count          = 1000267004,
	Vertex_Input_Binding_Stride = 1000267005,
	Depth_Test_Enable           = 1000267006,
	Depth_Write_Enable          = 1000267007,
	Depth_Compare_Op            = 1000267008,
	Depth_Bounds_Test_Enable    = 1000267009,
	Stencil_Test_Enable         = 1000267010,
	Stencil_Op                  = 1000267011,
	Rasterizer_Discard_Enable   = 1000377001,
	Depth_Bias_Enable           = 1000377002,
	Primitive_Restart_Enable    = 1000377004,
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
	std::span<const DynamicState> dynamicStates = {{DynamicState::ViewPort, DynamicState::Scissor}};
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
	ImageLayout   oldLayout           = ImageLayout::MaxEnum; // == use previous layout
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

struct ImagePair {
	const Image& colorAttachment;
	const Image* resolveImage = nullptr;
};

enum class LoadOp {
	Load = 0,
	Clear = 1,
	DontCare = 2,
	NoneExt = 1000400000,
	MaxEnum = 0x7FFFFFFF, 
};

struct RenderingInfo {
	const std::span<ImagePair const> colorAttachs;
	Image const& depthAttach = {};
	ivec4 renderArea = ivec4(0.0f);
	LoadOp loadOpColor = LoadOp::Clear;
	vec4 clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	float clearDepth = 1.0f;
	uint32_t clearStencil = 0;
	ivec4 scissor = ivec4(0);
	uint32_t layerCount = 1;
};

struct RegionPair {
	ivec4 dst;
	ivec4 src;
};

struct BlitInfo {
	Image dst;
	Image src;
	const std::span<const RegionPair> regions = {};
	Filter filter = Filter::Linear;
};

struct Command {
	std::shared_ptr<CommandResource> resource;
	Command();

	bool Copy(Buffer& dst, const void*   data, uint32_t size, uint32_t dstOffset = 0);
	void Copy(Buffer& dst, Buffer& src,  uint32_t size, uint32_t dstOffset = 0, uint32_t srcOffset = 0);
	bool Copy(Image&  dst, const void*   data, uint32_t size);
	void Copy(Image&  dst, Buffer& src,  uint32_t srcOffset = 0); // size is a No OP
	void Copy(Buffer& dst, Image& src, uint32_t dstOffset, ivec2 imageOffset, ivec2 imageExtent);// size is a No OP
	void Barrier(Image& img, const ImageBarrier& barrier);
	void Barrier(Buffer& buf, const BufferBarrier& barrier);
	void Barrier(const MemoryBarrier& barrier);
	void Blit(BlitInfo const& info);
	void ClearColorImage(Image& image, const float4& color);


	void BeginRendering(const RenderingInfo& info);
	void SetViewport(ivec4 const& viewport);
	void SetScissor(ivec4 const& scissor);
	void EndRendering();
	// void BeginPresent();
	// void EndPresent();
	void BindPipeline(Pipeline& pipeline);
	void PushConstants(const Pipeline& pipeline, const void* data, uint32_t size);

	// void BuildBLAS(BLAS& blas);
	// void BuildTLAS(TLAS& tlas, const std::vector<BLASInstance>& instances);

	void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
	void BindVertexBuffer(Buffer const& vertexBuffer);
	void BindIndexBuffer(Buffer const& indexBuffer);
	void DrawMesh(Buffer const& vertexBuffer, Buffer const& indexBuffer, uint32_t indexCount);
	void DrawLineStrip(Buffer const& pointsBuffer, uint32_t firstPoint, uint32_t pointCount, float thickness = 1.0f);
	void DrawPassThrough();
	void DrawImGui(void* imDrawData);
	void Dispatch(const uvec3& groups);

	int BeginTimeStamp(const std::string& name);
	void EndTimeStamp(int timeStampIndex);
	void Begin();
	void End();
	void WaitQueue();
	void QueueSubmit(const SubmitInfo& submitInfo);
};

struct Device {
	std::shared_ptr<DeviceResource> resource;
	Buffer CreateBuffer(const BufferDesc& desc);
	Image CreateImage(const ImageDesc& desc);
	Pipeline CreatePipeline(const PipelineDesc& desc);

	Queue GetQueue(QueueFlags flags);
	Command& GetCommandBuffer(const Queue& queue);
	
	void ResetStaging();
	u8* GetStagingPtr();
	u32& GetStagingOffset();
	u32 GetStagingSize();	

	void WaitQueue(Queue* queue);
	void WaitIdle();


	void* MapBuffer(Buffer& buffer);
	void UnmapBuffer(Buffer& buffer);
};


class SwapChain {
	std::shared_ptr<SwapChainResource> resource;

	std::vector<Image> swapChainImages;

	void* glfwWindow = nullptr;	
	uint32_t framesInFlight = 2;
	uint32_t additionalImages = 0;
	uint32_t currentFrame = 0;
	bool dirty = true;
	uint32_t currentImageIndex = 0;

	// SwapChain() = default;

public:
	// ~SwapChain() { printf("~SwapChain\n"); }

	inline Image& GetCurrentImage()   { return swapChainImages[currentImageIndex]; }
	Command&      GetCommandBuffer();

	void Create(Device& device, vkw::Queue& queue, void* glfwWindow, uvec2 const size);
	void CreateUI(SampleCount sampleCount);
	void Destroy();
	void Recreate(uvec2 const size);
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
Device CreateDevice(const std::span<Queue*> queues);
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