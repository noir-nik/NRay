#ifndef _INDEXING_H
#define _INDEXING_H

#include "GpuTypes.h"

#define EXISTS(_textureIndex) (_textureIndex != TEXTURE_UNDEFINED)

#ifdef GLSL

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_image_load_formatted : require

layout(set = 0, binding = BINDING_TEXTURE) uniform sampler2D textures[];
// layout(set = 0, binding = BINDING_TEXTURE) uniform samplerCube cubeTextures[];

layout(set = 0, binding = BINDING_BUFFER) buffer floatBuffer {
	float data[];
} floatBuffers[];

layout(set = 0, binding = BINDING_BUFFER) buffer float4Buffer {
	vec4 data[];
} float4Buffers[];

layout(set = 0, binding = BINDING_BUFFER) readonly buffer MaterialBuffer {
	Material materials[];
} materialBuffers[];

// layout(set = 0, binding = BINDING_STORAGE_IMAGE) uniform image2D images[];

#define MATERIAL materialBuffers[ctx.materialBufferRID].materials[ctx.materialOffset]
#endif // GLSL


#ifdef SLANG

[[vk::binding(BINDING_BUFFER, 0)]] RWStructuredBuffer<float> floatBuffers[];
[[vk::binding(BINDING_BUFFER, 0)]] RWStructuredBuffer<float4> float4Buffers[];
// [[vk::binding(BINDING_STORAGE_IMAGE, 0)]] RWTexture2D<float4> images[];
// [[vk::binding(BINDING_TLAS, 0)]] AccelerationStructure tlas;

#endif // SLANG

#endif // _INDEXING_H