#if (!defined SLANG) && (!defined GLSL)
#pragma once
#include <string>
#include <cstdint>
#include <vector>

#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.hpp"
#endif

using mat2  = Lmath::float4;
using vec2  = Lmath::float2;

struct WindowTestInfo
{
	int width;
	int height;
};

class WindowTestApplication {
public:
	void run(WindowTestInfo* pWindowTestInfo);
private:
	void Setup();
	void Create();
	void Draw();
	void Compute();
	void MainLoop();
	void Finish();

	WindowTestInfo* info;
};
#endif



struct WindowTestConstants {
	mat2 transform;

	vec2 offset;
	int pad[2];
};