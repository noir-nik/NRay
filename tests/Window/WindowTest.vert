#version 450
#extension GL_GOOGLE_include_directive : enable
#include "FeatureTest.hpp"

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform _constants {
	FeatureTestConstants ctx;
};


void main() {
	// float xScale = 1.;
	// float yScale = (ctx.width / ctx.height);
	// vec2 pos = vec2(inPosition.x * xScale, inPosition.y * yScale);
	vec2 pos  = ctx.transform * inPosition + ctx.offset;
    gl_Position = vec4(pos, 0.0, 1.0);
    fragColor = inColor;
}