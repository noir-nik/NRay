#pragma once
#include "Lmath.hpp"
#include <cstdint>

void fillUV(Lmath::float4* image, int width, int height);
void saveBuffer(const char *fname, void* buffer, uint32_t width, uint32_t height);