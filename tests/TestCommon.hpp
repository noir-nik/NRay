#pragma once
#ifdef USE_MODULES
import lmath;
#else
#include "lmath_types.hpp"
#endif
#include <cstdint>


void fillUV(lmath::float4* image, int width, int height);
