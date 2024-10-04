#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace vkw {

enum Queue {
    Graphics = 0,
    Compute = 1,
    Transfer = 2,
    Count = 3,
};



void Init();
void Destroy();
}
