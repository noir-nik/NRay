#pragma once
#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.cxx"
#endif
#include <cstdint>


void fillUV(Lmath::float4* image, int width, int height);
