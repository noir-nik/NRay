#version 450
#extension GL_GOOGLE_include_directive : enable
#include "FeatureTest.cppm"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform _constants {
	FeatureTestConstants ctx;
};


void main() {
	// float xScale = 1.0;
	// float yScale = (ctx.width / ctx.height);
	// vec2 pos = vec2(inPosition.x * xScale, inPosition.y * yScale);
	vec4 pos = ctx.proj * ctx.view * vec4(inPosition, 1.0);
	// vec4 pos = ctx.viewProj * vec4(inPosition, 1.0);
    gl_Position = pos;
    fragColor = inColor;
}