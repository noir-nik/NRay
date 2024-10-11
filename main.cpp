#include "Pch.hpp"
// #include "VulkanBase.hpp"

// #include "NeuralSdf.hpp"

#include "VulkanBase.hpp"

#include "NeuralSdf.hpp"

#include "FileManager.hpp"

#include <filesystem>

int main(int argc, char* argv[])
{
	Logger::Init();
	NeuralSdfApplication app;
	NeuralSdfInfo neuralSdfInfo = {
		.weightsPath = "assets/sdf1_weights.bin",
		.numLayers = 2,
		.layerSize = 64,
		.outputPath = "output.bmp",
		.width = 256,
		.height = 256,
	};
	app.run(&neuralSdfInfo);
	return 0;
}