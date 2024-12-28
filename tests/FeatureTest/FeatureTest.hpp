#if (!defined SLANG) && (!defined GLSL)
#pragma once

#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.hpp"
#endif

using mat2 = Lmath::float4;
using mat4 = Lmath::float4x4;
using vec2 = Lmath::float2;
using vec3 = Lmath::float3;

struct FeatureTestInfo
{
	int width;
	int height;
};

class FeatureTestApplication {
public:
	void run(FeatureTestInfo* pFeatureTestInfo);
private:
	void Create();
	void Setup();
	void Draw();
	void Compute();
	void MainLoop();
	void Finish();

	FeatureTestInfo* info;
};
#endif



struct FeatureTestConstants {
	mat4 view;
	mat4 proj;
	mat4 viewProj;
};

