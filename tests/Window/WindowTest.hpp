#if (!defined SLANG) && (!defined GLSL)
#pragma once
#ifdef USE_MODULES
import lmath;
#else
#include "lmath.hpp"
#endif
#include <string>
#include <cstdint>
#include <vector>


using mat2  = lmath::float4;
using vec2  = lmath::float2;

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