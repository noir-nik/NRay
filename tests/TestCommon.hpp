#pragma once
#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath_types.h"
#endif
#include <cstdint>


void fillUV(Lmath::float4* image, int width, int height);
