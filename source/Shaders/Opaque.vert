#version 460
#extension GL_GOOGLE_include_directive : enable

#include "Indexing.h"
#include "Opaque.h"

layout(location = 0) in vec3  inPosition;
layout(location = 1) in vec3  inNormal;
layout(location = 2) in vec2  inUV;
layout(location = 3) in vec4  inColor;
layout(location = 4) in vec4  inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 2) out vec2 outUV;
layout(location = 3) out mat3 outTBN;


layout(push_constant) uniform _constants {
	OpaqueConstants ctx;
};


void main() {
	vec4 worldPosition = ctx.model * vec4(inPosition, 1.0);
    outPosition = worldPosition.xyz;
	outUV = inUV;

	mat3 model3x3 = mat3(ctx.model);
	mat3 normalMatrix = transpose(inverse(model3x3));

	vec3 N = normalMatrix * inNormal;
	vec3 T = normalMatrix * inTangent.xyz;
	outTBN[0] = T;
	outTBN[1] = cross(N, T) * -inTangent.w;
	outTBN[2] = N;

    gl_Position = ctx.viewProj * worldPosition;
}