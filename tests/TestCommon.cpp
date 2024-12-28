#ifdef USE_MODULES
import Lmath.types;
import VulkanBackend;
#else
#include "VulkanBackend.cppm"
#include "TestCommon.hpp"
#include "FileManager.cppm"
#endif

using Pixel = Lmath::float4;

void fillUV(Lmath::float4* image, int width, int height){
	for (auto i = 0; i < height; i++) {
		for (auto j = 0; j < width; j++) {
			int idx = i * width + j;
			image[idx].r = i / (float)height;
			image[idx].g = j / (float)width;
			image[idx].b = 0.0f;
			image[idx].a = 1.0f;
		}
	}
}
