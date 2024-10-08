#include "Pch.hpp"
#include "VulkanBase.hpp"

#include "NeuralSdf.hpp"

int main()
{
	// print pwd
	std::cout << std::filesystem::current_path() << std::endl;
	Logger::Init();
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
	return 0;
}