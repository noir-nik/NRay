module;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// NOLINTBEGIN(misc-unused-using-decls)
export module stb_image;
export {
using ::stbi_load;
using ::stbi_image_free;
using ::stbi_load_from_memory;
}
// NOLINTEND(misc-unused-using-decls)