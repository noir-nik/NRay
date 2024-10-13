#if defined(ENGINE)
#pragma once
#include <string>
#include <cstdint>
#include <vector>

#include "Lmath.hpp"

struct RadienceFieldInfo
{
	int width;
	int height;
	const char* gridPath;
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

const int SH_WIDTH = 9;

struct Cell
{
	float density;
	float sh_r[SH_WIDTH];
	float sh_g[SH_WIDTH];
	float sh_b[SH_WIDTH];
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

#ifdef GLSL

layout(set = 0, binding = BINDING_BUFFER) buffer CellsBuffer {
    Cell data[];
} CellBuffers[];


#endif

#ifndef ENGINE

#define cells CellBuffers[ctx.gridRID].data

#endif