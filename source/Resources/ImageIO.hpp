#pragma once
#include <stb_image.h>

namespace ImageIO {

inline unsigned char* Load(const char* filename, int* width, int* height, int* channels, int desiredChannels) {
	return stbi_load(filename, width, height, channels, desiredChannels);
}

inline unsigned char* LoadFromMemory(const unsigned char* buffer, int len, int* width, int* height, int* channels, int desiredChannels) {
	return stbi_load_from_memory(buffer, len, width, height, channels, desiredChannels);
}

inline void Free(unsigned char* data) {
	stbi_image_free(data);
}

} // namespace ImageIO