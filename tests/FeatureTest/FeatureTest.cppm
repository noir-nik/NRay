#ifdef ENGINE
#ifdef USE_MODULES
export module FeatureTest;
#define _FEATURETEST_EXPORT export
import Lmath;
#else
#pragma once
#include "Lmath_types.h"
#define _FEATURETEST_EXPORT
#endif // USE_MODULES
using namespace Lmath;


_FEATURETEST_EXPORT
class FeatureTestApplication {
public:
	void run(const char* gltfPath);
private:
	void Create(const char* gltfPath);
	void Setup();
	void Draw();
	void Compute();
	void MainLoop();
	void Finish();
};
#else
#define _FEATURETEST_EXPORT
#endif // ENGINE



struct FeatureTestConstants {
	mat4 view;
	mat4 proj;
	mat4 viewProj;
};

