#define SLANG
#include "../../source/Shaders/Bindless.h"

[[vk::push_constant]]
SlangTestConstants ctx;

[shader("compute")]
[numthreads(WORKGROUP_SIZE, WORKGROUP_SIZE, 1)]
void main(uint3 tid: SV_DispatchThreadID) {
    if (tid.x < ctx.width && tid.y < ctx.height){
		uint index = tid.y * ctx.width + tid.x;
        outputImage[index] = float4(0.49, 0.73, 0.03, 1.0);
	}
}