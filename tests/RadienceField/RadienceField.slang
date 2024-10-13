#define SLANG
#include "../../source/Shaders/ShaderCommon.h"
#include "RadienceField.hpp"

#define ImageOpt float4Buffers[ctx.imageOptRID]
#define ImageGT float4Buffers[ctx.imageGTRID]
#define grad float4Buffers[ctx.gradRID]

[[vk::push_constant]]
ImageOptConstants ctx;


[shader("compute")]
[numthreads(WORKGROUP_SIZE, WORKGROUP_SIZE, 1)]
void main(uint3 tid: SV_DispatchThreadID) {
	if (tid.x < ctx.width && tid.y < ctx.height){
		uint index = tid.y * ctx.width + tid.x;

		ImageOpt[index] -= grad[index] * ctx.learningRate;
		
	}
}

// slangc.exe .\tests\RadienceField\RadienceField.slang -entry main -Wno-39001 -line-directive-mode none -o ImgOptMse.glsl