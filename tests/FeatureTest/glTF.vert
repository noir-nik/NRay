#version 450
#extension GL_GOOGLE_include_directive : enable
#include "FeatureTest.hpp"

// layout(location = 0) in vec3 position;
// layout(location = 1) in vec3 color;

layout(location = 0) in vec3  position;
layout(location = 1) in vec3  normal;
layout(location = 2) in vec2  uv;
layout(location = 3) in vec4  color;
layout(location = 4) in vec3  tangent;


layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform _constants {
	FeatureTestConstants ctx;
};


void main() {
	// float xScale = 1.;
	// float yScale = (ctx.width / ctx.height);
	// vec2 pos = vec2(position.x * xScale, position.y * yScale);
	vec4 pos = ctx.proj * ctx.view * ctx.model * vec4(position, 1.0);
    gl_Position = pos;
    // fragColor = color;
    fragColor = vec3(0.5);
}