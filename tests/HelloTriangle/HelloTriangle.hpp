#if defined(ENGINE)
#pragma once
#include <string>
#include <cstdint>
#include <vector>

#include "Lmath.hpp"
using float3 = Lmath::float3;
using float4x4 = Lmath::float4x4;


struct HelloTriangleInfo
{
	int width;
	int height;
};

class HelloTriangleApplication {
public:
	void run(HelloTriangleInfo* pHelloTriangleInfo);
private:
	void Setup();
	void Create();
	void Draw();
	void Finish();

	HelloTriangleInfo* info;
};
#endif

struct HelloTriangleConstants {
	int width;
	int height;
	int outputImageRID;
	int pad[1];
};
