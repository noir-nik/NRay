#define SLANG
#include "../../source/Shaders/Bindless.h"
#include "ImageOptimization.hpp"

#define ImageOpt float4Buffers[ctx.imageOptRID]
#define ImageGT float4Buffers[ctx.imageGTRID]
#define grad float4Buffers[ctx.gradRID]

[[vk::push_constant]]
ImageOptConstants ctx;

float4 d_mse(const float4 param, const float4 gt) {
	return { (param.x - gt.x), (param.y - gt.y), (param.z - gt.z), 0.0f };
}

// [BackwardDifferentiable]
// float msePerPixel(const float4 a, no_diff float4 b) {
//     return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
// }

[BackwardDifferentiable]
float msePerPixel(const float4 a, no_diff float4 b) {
    float res = ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
    return res;
}

// void with out float res
// [BackwardDifferentiable]
// void msePerPixel(const float4 a, no_diff float4 b, out float res) {
//     res = ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
// }

[BackwardDifferentiable]
void msePerPixelAdd(const float4 a, no_diff float4 b, out float res) {
    res += ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

// ? How to use this?
[BackwardDifferentiable]
void mse(no_diff int width, no_diff int height, out float res) {
    float sum = 0.0;
    [MaxIters(256)]
	// [ForceUnroll]
    for (int y = 0; y < height; ++y) {
        [MaxIters(256)]
		// [ForceUnroll]
		for (int x = 0; x < width; ++x) {
			msePerPixelAdd(ImageOpt[y * width + x], ImageGT[y * width + x], sum);
		}
	}
	sum /= (width * height * 6.0);
	res = sum;
}

typedef DifferentialPair<float> dpfloat;
typedef DifferentialPair<float4> dpfloat4;


[shader("compute")]
[numthreads(WORKGROUP_SIZE, WORKGROUP_SIZE, 1)]
void main(uint3 tid: SV_DispatchThreadID) {
	if (tid.x < ctx.width && tid.y < ctx.height){
		uint index = tid.y * ctx.width + tid.x;
		for (int iter = 0; iter < ctx.numIterations; ++iter) {

			grad[index] = d_mse(ImageOpt[index], ImageGT[index]);
			let a = ImageOpt[index];
			let b = ImageGT[index];
            dpfloat4 dpa = diffPair(ImageOpt[index]);
            dpfloat4 dpb = diffPair(ImageGT[index]);

            dpfloat dpo = dpfloat(2.0, 1.0);
            // let result = fwd_diff(msePerPixel)(diffPair(a, 1), diffPair(b, 0));
            // grad[index] = sqrt(result.d);
            // bwd_diff(msePerPixel)(a, b, float(1.0));
            bwd_diff(msePerPixel)(dpa, b, float(1.0));

            // bwd_diff(mse)(ctx.width, ctx.height, float(1.0));
			
            grad[index] = dpa.d;

            ImageOpt[index] -= grad[index] * ctx.learningRate;
		}
	}
}

// slangc.exe .\tests\ImageOptimization\ImageOptimization.slang -entry main -Wno-39001 -line-directive-mode none -o ImgOptMse.glsl