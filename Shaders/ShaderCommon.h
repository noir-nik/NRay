#define BINDING_TEXTURE 0 // Sampled image
#define BINDING_BUFFER 1
#define BINDING_TLAS 2
#define BINDING_STORAGE_IMAGE 3

#define WORKGROUP_SIZE 16

struct imageOptConstants {
	int width;
	int height;
	int factor; //test
	int pad[1];
};

struct NeuralSdfConstants {
	int width;
	int height;
	int numLayers;
	int layerSize;

	int weightsRID;
	int outputImageRID;
	int pad[2];
};

// for shaders only
#ifndef ENGINE

#extension GL_ARB_separate_shader_objects : enable
// #extension GL_EXT_nonuniform_qualifier : enable


layout(set = 0, binding = BINDING_TEXTURE) uniform sampler2D textures[];
layout(set = 0, binding = BINDING_TEXTURE) uniform samplerCube cubeTextures[];


layout(set = 0, binding = BINDING_BUFFER) readonly buffer WeightsBuffer {
    float data[];
} WeightsBuffers[];

layout(set = 0, binding = BINDING_BUFFER) readonly buffer OutImageBuffer {
    float res[];
} OutImageBuffers[];


layout(binding = BINDING_STORAGE_IMAGE) uniform image2D images[];

#endif