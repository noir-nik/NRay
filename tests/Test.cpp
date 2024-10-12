#include "Test.hpp"

#include "NeuralSdf/NeuralSdf.hpp"
// #include "SlangTest/SlangTest.hpp"
#include "ImageOptimization/ImageOptimization.hpp"

namespace Test {
static void NeuralSdfTest();
static void ImageOptimizationTest();

void Test(TestName testName) {
	switch (testName)
	{
	case NeuralSdf:
		NeuralSdfTest();
		break;

	case Slang:
		ImageOptimizationTest();
		break;
	
	default:
		break;
	}
}

static void NeuralSdfTest() {
	NeuralSdfApplication app;
	NeuralSdfInfo neuralSdfInfo = {
		.weightsPath = "tests/NeuralSdf/sdf1_weights.bin",
		.numLayers = 2,
		.layerSize = 64,
		.width = 512,
		.height = 512,
	};
	app.run(&neuralSdfInfo);
}

static void ImageOptimizationTest() {
	ImageOptimizationApplication app;
	ImageOptimizationInfo imageOptimizationInfo = {
		.width = 512,
		.height = 512,
		.learningRate = 0.05f,
		.numIterations = 100,
	};
	app.run(&imageOptimizationInfo);
}


}