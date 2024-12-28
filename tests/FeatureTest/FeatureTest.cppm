#ifdef ENGINE
#ifdef USE_MODULES
export module FeatureTest;
#define _FEATURETEST_EXPORT export
import Lmath;
#else
#pragma once
#include "Lmath.cppm"
#define _FEATURETEST_EXPORT
#endif // USE_MODULES
using namespace Lmath;

_FEATURETEST_EXPORT
struct FeatureTestInfo
{
	int width;
	int height;
};

_FEATURETEST_EXPORT
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
#else
#define _FEATURETEST_EXPORT
#endif // ENGINE



struct FeatureTestConstants {
	mat4 view;
	mat4 proj;
	mat4 viewProj;
};

