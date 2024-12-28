#version 450
#extension GL_GOOGLE_include_directive : enable
#include "Opaque.cppm"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform _constants {
	OpaqueConstants ctx;
};

void main() {
	outColor = vec4(inPosition, 1.0); 
}