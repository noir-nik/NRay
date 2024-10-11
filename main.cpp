#include "Pch.hpp"
// #include "VulkanBase.hpp"

// #include "NeuralSdf.hpp"

#include "VulkanBase.hpp"

#include "NeuralSdf.hpp"
#include "SlangTest.hpp"

#include "FileManager.hpp"

#include <filesystem>

// int main(int argc, char* argv[])
// {
// 	Logger::Init();
// 	SlangTestApplication app;
// 	SlangTestInfo SlangTestInfo = {
// 		.width = 512,
// 		.height = 512,
// 	};
// 	app.run(&SlangTestInfo);
// 	return 0;
// }

// Example
int main(int argc, char* argv[])
{
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