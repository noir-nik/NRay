#if defined(ENGINE)
#pragma once
#ifdef USE_MODULES
import lmath;
#else
#include "lmath.hpp"
#endif
#include <string>
#include <cstdint>
#include <vector>

using float3 = lmath::float3;
using float4x4 = lmath::float4x4;


struct RadienceFieldInfo
{
	int width;
	int height;
	char const* gridPath;
	int gridSize;
};

class RadienceFieldApplication {
public:
	void run(RadienceFieldInfo* pRadienceFieldInfo);
private:
	void Setup();
	void Create();
	void Compute();
	void CmdOptimizationStep();
	void Finish();

	RadienceFieldInfo* info;
};
#endif

#define _SH_WIDTH 9

struct Cell
{
	float density;
	float sh_r[_SH_WIDTH];
	float sh_g[_SH_WIDTH];
	float sh_b[_SH_WIDTH];
};

struct RadienceFieldConstants {
	int width;
	int height;
	int outputImageRID;
	int gridRID;

	float3 bBoxMin;
	int gridSize;

	float3 bBoxMax;
	int pad1[1];

	int pad[4];

	float4x4 worldViewInv;
	float4x4 worldViewProjInv;
};
