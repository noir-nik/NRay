#pragma once
#include "Lmath.hpp"

void fillUV(float4* image, int width, int height);
void saveBuffer(const char *fname, void* buffer, uint32_t width, uint32_t height);