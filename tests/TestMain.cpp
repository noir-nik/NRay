#include "TestMain.hpp"
namespace Test {
void Test(TestName testName) {
	switch (testname)
	{
	case NeuralSdf:
		break;
	
	default:
		break;
	}
}

static void neuralSdfTest() {
	NeuralSdfApplication app;
	NeuralSdfInfo neuralSdfInfo = {
		.weightsPath = "assets/sdf1_weights.bin",
		.numLayers = 2,
		.layerSize = 64,
		.outputPath = "output.bmp",
		.width = 512,
		.height = 512,
	};
	app.run(&neuralSdfInfo);
}

}