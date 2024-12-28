#if defined(ENGINE)
#pragma once

#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.hpp"
#endif

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
	void MainLoop();
	void Finish();

	HelloTriangleInfo* info;
};
#endif

struct HelloTriangleConstants {
	int width;
	int height;
	int storageImageRID;
	int pad[1];
};
