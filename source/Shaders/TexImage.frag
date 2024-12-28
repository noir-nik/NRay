#version 450
#extension GL_GOOGLE_include_directive : enable
#include "Bindless.h"

layout (location = 3) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

// layout(push_constant) uniform _constants {
// 	TextureConstants ctx;
// };


void main() 
{
	// outFragColor = texture(textures[ctx.albedo],inUV);
	outFragColor = vec4(inUV, 0.0, 1.0);
}