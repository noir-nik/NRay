#pragma once
namespace Test {
enum TestName {
	NeuralSdf,
	ImageOptimization,
	Feature,
	RadienceField
};

void Test(TestName testName);

}