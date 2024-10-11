#include "ShaderCommon.h"

[[vk::push_constant]]
SlangTestConstants ctx;

[shader("compute")]
[numthreads(16, 16, 1)] // Define the thread group size
void main(uint3 tid: SV_DispatchThreadID) {
    if (tid.x < ctx.width && tid.y < ctx.height){
		uint index = tid.y * ctx.width + tid.x;
		Pixel redPixel;
		redPixel.value = float4(1.0, 0.0, 0.0, 1.0);

		OutImageBuffers[ctx.outputImageRID][index].value = float4(0.9, 0.3, 0.1, 1.0);
	}
}