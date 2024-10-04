#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace vkw {

struct Buffer {
    std::shared_ptr<BufferResource> resource;
    uint32_t size;
    BufferUsageFlags usage;
    MemoryFlags memory;
    uint32_t RID();
};

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



void Init();
void Destroy();
}
