#version 450
#extension GL_GOOGLE_include_directive : enable
#include "indexing.h"

layout (location = 3) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = texture(textures[0],inUV);
}