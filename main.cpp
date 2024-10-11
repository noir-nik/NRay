#include "Pch.hpp"
// #include "VulkanBase.hpp"

// #include "NeuralSdf.hpp"

#include "VulkanBase.hpp"

#include "NeuralSdf.hpp"

#include "FileManager.hpp"

#include <filesystem>

int main(int argc, char* argv[])
{	
	int numLayers, layerSize;
	// print pwd
	std::cout << std::filesystem::current_path() << std::endl;
	if (argc != 3) return 1;

	numLayers = atoi(argv[1]);
	layerSize = atoi(argv[2]);

	Logger::Init();
	NeuralSdfApplication app;
	NeuralSdfInfo neuralSdfInfo = {
		.weightsPath = "assets/my_weights"+std::to_string(numLayers)+"_"+std::to_string(layerSize)+".bin",
		.numLayers = numLayers,
		.layerSize = layerSize,
		.outputPath = "output.bmp",
		.width = 256,
		.height = 256,
	};
	app.run(&neuralSdfInfo);
	return 0;
}