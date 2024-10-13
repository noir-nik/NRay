#if (!defined SLANG) && (!defined GLSL)
#pragma once
#include <string>
#include <cstdint>
#include <vector>

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


const size_t SH_WIDTH = 9;

struct Cell
{
	float density;
	float sh_r[SH_WIDTH];
	float sh_g[SH_WIDTH];
	float sh_b[SH_WIDTH];
};

struct BoundingBox
{
	float3 min;
	float3 max;
};

struct RadienceFieldConstants {
	int width;
	int height;
	int outputImageRID;
	int gridRID;

	int gridSize;
	int pad[3];

	float4x4 worldViewInv;
	float4x4 worldViewProjInv;
};