#include "Pch.hpp"
#include "TestCommon.hpp"
#include "Lmath.hpp"
#include "FileManager.hpp"
#include "VulkanBase.hpp"

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

void saveBuffer(const char *fname, void* buffer, uint32_t width, uint32_t height) {
	vkw::Buffer* b = (vkw::Buffer*)(buffer);
	std::vector<unsigned char> image;
	image.reserve(width * height * 4);
	Pixel* mappedMemory = (Pixel*)vkw::MapBuffer(*b);
	for (int i = 0; i < width * height; i++) {
		image.push_back(255.0f * mappedMemory[i].r);
		image.push_back(255.0f * mappedMemory[i].g);
		image.push_back(255.0f * mappedMemory[i].b);
		image.push_back(255.0f);
	}
	vkw::UnmapBuffer(*b);
	FileManager::SaveBMP(fname, (const uint32_t*)image.data(), width, height);
}