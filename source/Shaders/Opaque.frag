#version 460
#extension GL_GOOGLE_include_directive : enable
#include "Opaque.h"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform _constants {
	OpaqueConstants ctx;
};

void main() {
	// vec3 col = vec3(inPosition);
	// vec3 col = MATERIAL.emissiveFactor;
	vec3 col = MATERIAL.baseColorFactor.xyz;
	col = pow(col, vec3(1.0/2.2));
	outColor = vec4(col, 1.0); 
}