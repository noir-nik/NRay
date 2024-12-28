#pragma once
#ifdef USE_MODULES
import Lmath.types;
#else
#include "Lmath.types.cppm"
#endif
#include <cstdint>


void fillUV(Lmath::float4* image, int width, int height);
