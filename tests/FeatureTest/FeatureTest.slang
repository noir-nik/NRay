#define SLANG
#include "../../source/Shaders/ShaderCommon.h"
#include "FeatureTest.hpp"

[[vk::push_constant]]
FeatureTestConstants ctx;

[shader("compute")]
[numthreads(WORKGROUP_SIZE, WORKGROUP_SIZE, 1)]
void main(uint3 tid: SV_DispatchThreadID, uint3 localId: SV_GroupThreadID, uint3 groupID: SV_GroupID, uint localIndex: SV_GroupIndex) {
    if (tid.x < ctx.width && tid.y < ctx.height){
        uint index = tid.y * ctx.width + tid.x;
        // outputImage[index] = float4(localId.x / float(WORKGROUP_SIZE), localId.y / float(WORKGROUP_SIZE), 0.0, 1.0);
        // outputImage[index] = float4(groupID.x / float(ctx.width) * WORKGROUP_SIZE, groupID.y / float(ctx.height) * WORKGROUP_SIZE, 0.0, 1.0);
        // float r = localIndex + WORKGROUP_SIZE * WORKGROUP_SIZE * groupID;
        outputImage[index] = float4((index % ctx.width) / float(ctx.width), 0.0, 0.0, 1.0);
	}
}