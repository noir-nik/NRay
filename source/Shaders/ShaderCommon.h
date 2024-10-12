#define BINDING_TEXTURE 0 // Sampled image
#define BINDING_BUFFER 1
#define BINDING_TLAS 2
#define BINDING_STORAGE_IMAGE 3

#define WORKGROUP_SIZE 16

struct NeuralSdfConstants {
	int width;
	int height;
	int numLayers;
	int layerSize;

	int weightsRID;
	int outputImageRID;
	int imageGPU;
	int pad[1];
};

struct SlangTestConstants{
	int width;
	int height;
	int outputImageRID;
	int pad[1];
};

struct ImageOptConstants {
	int width;
	int height;
	int imageOptRID;
	int imageGTRID;

	int gradRID;
	float learningRate;
	int pad[2];
};



#ifdef GLSL

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_image_load_formatted : require

struct Pixel{
  vec4 value;
};

// layout(set = 0, binding = BINDING_TEXTURE) uniform sampler2D textures[];
// layout(set = 0, binding = BINDING_TEXTURE) uniform samplerCube cubeTextures[];


layout(set = 0, binding = BINDING_BUFFER) buffer WeightsBuffer {
    float data[];
} WeightsBuffers[];

layout(set = 0, binding = BINDING_BUFFER) buffer OutImageBuffer {
    Pixel data[];
} OutImageBuffers[];

layout(binding = BINDING_STORAGE_IMAGE) uniform image2D images[];

// Neural SDF
#define w_b WeightsBuffers[ctx.weightsRID].data
#define outputImage OutImageBuffers[ctx.outputImageRID].data

#endif


#ifdef SLANG

// struct Pixel {
//     float4 value;
// };

[[vk::binding(BINDING_BUFFER, 0)]] RWStructuredBuffer<float4> float4Buffers[];
[[vk::binding(BINDING_BUFFER, 0)]] RWStructuredBuffer<float> floatBuffers[];
[[vk::binding(BINDING_STORAGE_IMAGE, 0)]] RWTexture2D<float4> images[];

#define w_b floatBuffers[ctx.weightsRID]
#define outputImage float4Buffers[ctx.outputImageRID]

#endif


#ifndef ENGINE



#endif