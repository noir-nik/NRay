#if (!defined SLANG) && (!defined GLSL)
#pragma once
#include <string>
#include <cstdint>
#include <vector>

#include "Lmath.hpp"

using mat2 = Lmath::float4;
using mat4 = Lmath::float4x4;
using vec2 = Lmath::float2;

struct FeatureTestInfo
{
	int width;
	int height;
};

class FeatureTestApplication {
public:
	void run(FeatureTestInfo* pFeatureTestInfo);
private:
	void Setup();
	void Create();
	void Draw();
	void Compute();
	void MainLoop();
	void Finish();

	FeatureTestInfo* info;
};
#endif



struct FeatureTestConstants {
	mat4 model;
	mat4 view;
	mat4 proj;
	// mat4 viewproj;

	// vec2 offset;
	// int pad[2];
};