#ifdef ENGINE
#ifdef USE_MODULES
export module FeatureTest;
#define _FEATURETEST_EXPORT export
import lmath;
#else
#pragma once
#include "lmath_types.hpp"
#define _FEATURETEST_EXPORT
#endif // USE_MODULES
using namespace lmath;

_FEATURETEST_EXPORT
class FeatureTestApplication {
public:
	void run(char const* gltfPath);
private:
	void Create(char const* gltfPath);
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

