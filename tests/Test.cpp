#include "Test.hpp"

#include "NeuralSdf/NeuralSdf.hpp"
#include "SlangTest/SlangTest.hpp"

namespace Test {
static void neuralSdfTest();
static void slangTest();

void Test(TestName testName) {
	switch (testName)
	{
	case NeuralSdf:
		neuralSdfTest();
		break;

	case Slang:
		slangTest();
		break;
	
	default:
		break;
	}
}

static void neuralSdfTest() {
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

static void slangTest() {
	SlangTestApplication app;
	SlangTestInfo SlangTestInfo = {
		.width = 512,
		.height = 512,
		.learningRate = 0.05f
	};
	app.run(&SlangTestInfo);
}


}