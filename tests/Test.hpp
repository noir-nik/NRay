// #ifdef USE_MODULES
// export module Test;
// #define _TEST_EXPORT export
// #else
#pragma once
// #define _TEST_EXPORT

// #endif

// _TEST_EXPORT
namespace Test {
enum TestName {
	NeuralSdf,
	ImageOptimization,
	Feature,
	RadienceField,
	HelloTriangle
};

void Test(TestName testName);

}