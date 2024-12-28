#pragma once
#include <cstdint>

#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.hpp"
#endif

void fillUV(Lmath::float4* image, int width, int height);
