#ifdef USE_MODULES
module;
#else
#pragma once
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// NOLINTBEGIN(misc-unused-using-decls)
#ifdef USE_MODULES
export module stb_image;

export using ::stbi_load;
export using ::stbi_image_free;
export using ::stbi_load_from_memory;
#endif
// NOLINTEND(misc-unused-using-decls)