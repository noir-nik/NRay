#ifdef USE_MODULES
export module ImageIO;
#define _IMAGEIO_EXPORT export
import stb_image;
#else
#pragma once
#define _IMAGEIO_EXPORT
#include "stb_image.cppm"

#endif

_IMAGEIO_EXPORT
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