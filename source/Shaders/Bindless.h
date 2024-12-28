#ifndef BINDLESS_H
#define BINDLESS_H

#ifdef ENGINE
#include "Lmath_types.h"
using Lmath::vec4;
using Lmath::vec3;
using Lmath::vec2;
using Lmath::uint;
#endif // ENGINE

#include "GpuTypes.h"

#define BINDING_TEXTURE 0 // Sampled image
#define BINDING_BUFFER 1
#define BINDING_TLAS 2
#define BINDING_STORAGE_IMAGE 3

#define WORKGROUP_SIZE 16

#define MAX_MODELS 256

#ifndef ENGINE
#define EXISTS(textureIndex) (textureIndex != TEXTURE_UNDEFINED)
#endif // ENGINE

#ifdef GLSL

#define float3 vec3
#define float4x4 mat4

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

layout(set = 0, binding = BINDING_STORAGE_IMAGE) uniform image2D images[];

#define MATERIAL materialBuffers[ctx.materialBufferRID].materials[ctx.materialOffset]
#endif // GLSL


#ifdef SLANG

[[vk::binding(BINDING_BUFFER, 0)]] RWStructuredBuffer<float> floatBuffers[];
[[vk::binding(BINDING_BUFFER, 0)]] RWStructuredBuffer<float4> float4Buffers[];
[[vk::binding(BINDING_STORAGE_IMAGE, 0)]] RWTexture2D<float4> images[];
// [[vk::binding(BINDING_TLAS, 0)]] AccelerationStructure tlas;

#endif // SLANG

#endif // BINDLESS_H