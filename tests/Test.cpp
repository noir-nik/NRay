#include "Test.hpp"

#include "NeuralSdf/NeuralSdf.hpp"
#include "ImageOptimization/ImageOptimization.hpp"
#include "FeatureTest/FeatureTest.hpp"
#include "RadienceField/RadienceField.hpp"
#include "HelloTriangle/HelloTriangle.hpp"

namespace Test {
static void NeuralSdfTest();
static void ImageOptimizationTest();
static void RadienceFieldTest();
static void FeatureTest();
static void HelloTriangleTest();

void Test(TestName testName) {
	switch (testName)
	{
	case NeuralSdf: NeuralSdfTest(); break;
	case ImageOptimization: ImageOptimizationTest(); break;
	case Feature: FeatureTest(); break;
	case RadienceField: RadienceFieldTest(); break;
	case HelloTriangle: HelloTriangleTest(); break;
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
		.width = 640*2,
		.height = 480*2,
	};
	app.run(&FeatureTestInfo);
}

static void RadienceFieldTest() {
	RadienceFieldApplication app;
	RadienceFieldInfo RadienceFieldInfo = {
		.width = 256,
		.height = 256,
		.gridPath = "bin/model.dat",
		.gridSize = 128,
	};
	app.run(&RadienceFieldInfo);
}

static void HelloTriangleTest() {
	HelloTriangleApplication app;
	HelloTriangleInfo HelloTriangleInfo = {
		// .width = 640,
		// .height = 480,
		.width = 320,
		.height = 180,
	};
	app.run(&HelloTriangleInfo);
}

}