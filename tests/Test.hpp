#pragma once
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