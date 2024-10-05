#pragma once
#include <vulkan/vulkan.h>

#include <vector>
#include <filesystem>
namespace vkw {

using Flags = uint32_t;

enum Memory {
    GPU = 0x00000001,
    CPU = 0x00000002 | 0x00000004,
};
using MemoryFlags = Flags;

namespace BufferUsage {
    enum {
        TransferSrc = 0x00000001,
        TransferDst = 0x00000002,
        UniformTexel = 0x00000004,
        StorageTexel = 0x00000008,
        Uniform = 0x00000010,
        Storage = 0x00000020,
        Index = 0x00000040,
        Vertex = 0x00000080,
        Indirect = 0x00000100,
        Address = 0x00020000,
        VideoDecodeSrc = 0x00002000,
        VideoDecodeDst = 0x00004000,
        TransformFeedback = 0x00000800,
        TransformFeedbackCounter = 0x00001000,
        ConditionalRendering = 0x00000200,
        AccelerationStructureInput = 0x00080000,
        AccelerationStructure = 0x00100000,
        ShaderBindingTable = 0x00000400,
        SamplerDescriptor = 0x00200000,
        ResourceDescriptor = 0x00400000,
        PushDescriptors = 0x04000000,
        MicromapBuildInputReadOnly = 0x00800000,
        MicromapStorage = 0x01000000,
    };
}
using BufferUsageFlags = Flags;

enum Format {
    RGBA8_unorm = 37,
    BGRA8_unorm = 44,
    RG32_sfloat = 103,
    RGB32_sfloat = 106,
    RGBA32_sfloat = 109,
    D32_sfloat = 126,
    D24_unorm_S8_uint = 129,
};

namespace ImageUsage {
    enum {
        TransferSrc = 0x00000001,
        TransferDst = 0x00000002,
        Sampled = 0x00000004,
        Storage = 0x00000008,
        ColorAttachment = 0x00000010,
        DepthAttachment = 0x00000020,
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

namespace Layout {
    enum ImageLayout {
        Undefined = 0,
        General = 1,
        ColorAttachment = 2,
        DepthStencilAttachment = 3,
        DepthStencilRead = 4,
        ShaderRead = 5,
        TransferSrc = 6,
        TransferDst = 7,
        DepthReadStencilAttachment = 1000117000,
        DepthAttachmentStencilRead = 1000117001,
        DepthAttachment = 1000241000,
        DepthRead = 1000241001,
        StencilAttachment = 1000241002,
        StencilRead = 1000241003,
        Read = 1000314000,
        Attachment = 1000314001,
        Present = 1000001002,
    };
}

struct BufferResource;
struct ImageResource;
struct PipelineResource;

struct Buffer {
    std::shared_ptr<BufferResource> resource;
    uint32_t size;
    BufferUsageFlags usage;
    MemoryFlags memory;
    uint32_t RID();
};

// struct Image {
//     std::shared_ptr<ImageResource> resource;
//     uint32_t width = 0;
//     uint32_t height = 0;
//     ImageUsageFlags usage;
//     Format format;
//     Layout::ImageLayout layout;
//     AspectFlags aspect;
//     uint32_t layers = 1;
//     uint32_t RID();
//     // ImTextureID ImGuiRID();
//     // ImTextureID ImGuiRID(uint32_t layer);
// };

enum Queue {
    Graphics = 0,
    Compute = 1,
    Transfer = 2,
    Count = 3,
};

namespace PipelinePoint {
    enum Point {
        Graphics = 0,
        Compute = 1,
    };
}

namespace ShaderStage {
    enum Stage {
        Vertex = 0x00000001,
        Geometry = 0x00000008,
        Fragment = 0x00000010,
        Compute = 0x00000020,
        AllGraphics = 0x0000001F,
        All = 0x7FFFFFFF,
    };
}

struct Pipeline {
    struct Stage {
        ShaderStage::Stage stage;
        std::filesystem::path path;
        std::string entryPoint = "main";
    };
    PipelinePoint::Point point;
    std::shared_ptr<PipelineResource> resource;
    std::vector<Stage> stages;
    std::vector<std::vector<char>> stageBytes;
};

struct PipelineDesc {
    PipelinePoint::Point point;
    std::vector<Pipeline::Stage> stages;
    std::string name = "";
    std::vector<Format> vertexAttributes;
    std::vector<Format> colorFormats;
    bool useDepth = false;
    Format depthFormat;
    bool cullFront = false;
    bool lineTopology = false;
};


Buffer CreateBuffer(uint32_t size, BufferUsageFlags usage, MemoryFlags memory = Memory::GPU, const std::string& name = "");
// Image CreateImage(const ImageDesc& desc);
Pipeline CreatePipeline(const PipelineDesc& desc);


void Init();
void Destroy();
}
