#include "Test.hpp"

#include "NeuralSdf/NeuralSdf.hpp"
// #include "SlangTest/SlangTest.hpp"
#include "ImageOptimization/ImageOptimization.hpp"
#include "FeatureTest/FeatureTest.hpp"

namespace Test {
static void NeuralSdfTest();
static void ImageOptimizationTest();
static void FeatureTest();

void Test(TestName testName) {
	switch (testName)
	{
	case NeuralSdf: NeuralSdfTest(); break;
	case ImageOptimization: ImageOptimizationTest(); break;
	case Feature: FeatureTest(); break;
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
		.width = 256,
		.height = 256,
		.learningRate = 0.2f,
		.numIterations = 1,
	};
	app.run(&imageOptimizationInfo);
}

static void FeatureTest() {
	FeatureTestApplication app;
	FeatureTestInfo FeatureTestInfo = {
		.width = 64,
		.height = 64,
	};
	app.run(&FeatureTestInfo);
}


}