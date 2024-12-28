#version 450
#extension GL_GOOGLE_include_directive : enable
#include "Phong.h"

#include "inverse.glsl"
#include "transpose.glsl"

layout(location = 0) in vec3  inPosition;
layout(location = 1) in vec3  inNormal;
layout(location = 2) in vec2  inUV;
layout(location = 3) in vec4  inColor;
layout(location = 4) in vec3  inTangent;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;


// layout(location = 4) out vec3 fragTangent;

layout(push_constant) uniform _constants {
	PhongConstants ctx;
};


void main() {
	vec4 worldPosition = ctx.model * vec4(inPosition, 1.0);
    fragPosition = worldPosition.xyz;

	mat3 normalMatrix = transpose3(inverse3(mat3(ctx.model)));
	fragNormal = normalMatrix * inNormal;
    gl_Position = ctx.viewProj * worldPosition;
	fragUV = inUV;
}