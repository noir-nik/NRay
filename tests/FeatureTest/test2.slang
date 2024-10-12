#define SLANG
#include "../../source/Shaders/ShaderCommon.h"
#include "ImageOptimization.hpp"

#define ImageOpt float4Buffers[ctx.imageOptRID]
#define ImageGT float4Buffers[ctx.imageGTRID]
#define grad float4Buffers[ctx.gradRID]

[[vk::push_constant]]
ImageOptConstants ctx;

float4 d_mse(const float4 param, const float4 gt) {
    return { (param.x - gt.x), (param.y - gt.y), (param.z - gt.z), 0.0f };
}

float msePerPixel (const float4 a, const float4 b) {
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
}

[shader("compute")]
[numthreads(WORKGROUP_SIZE, WORKGROUP_SIZE, 1)]
void main(uint3 tid: SV_DispatchThreadID) {
    if (tid.x < ctx.width && tid.y < ctx.height){
        uint index = tid.y * ctx.width + tid.x;
        for (int i = 0; i < ctx.numIterations; ++i) {
			grad[index] = d_mse(ImageOpt[index], ImageGT[index]);
			ImageOpt[index] -= grad[index] * ctx.learningRate;
		}
	}
}