#include "Pch.hpp"
// #include "VulkanBase.hpp"

// #include "NeuralSdf.hpp"

#include "VulkanBase.hpp"

#include "NeuralSdf.hpp"

#include "FileManager.hpp"

#include <filesystem>

// struct NeuralSdfInfo
// {
// 	std::string weightsPath;
// 	int numLayers; // Number of hidden layers
// 	int layerSize; // Size of a hidden layer
// 	std::string outputPath;
// 	int width;
// 	int height;
// };

// int main()
// {
// 	// // print pwd
// 	std::cout << std::filesystem::current_path() << std::endl;
// 	Logger::Init();
// 	// NeuralSdfApplication app;
// 	NeuralSdfInfo ctx = {
// 		.weightsPath = "assets/sdf1_weights.bin",
// 		.numLayers = 2,
// 		.layerSize = 64,
// 		.outputPath = "output.bmp",
// 		.width = 256,
// 		.height = 256,
// 	};
// 	// std::cout << std::filesystem::current_path().c_str() << "\n";
// 	// app.run(&neuralSdfInfo);
// 	// return 0;
// 	int num_parameters = (3*ctx.layerSize + ctx.numLayers*ctx.layerSize*ctx.layerSize + ctx.layerSize) + (ctx.layerSize*(ctx.numLayers + 1) + 1);

// 	std::vector<float> weights = FileManager::ReadFloats(ctx.weightsPath);
// 	ASSERT(weights.size() == num_parameters, "Invalid number of weights, expected {0}, got {1}", num_parameters, weights.size());

// 	vkw::ComputeApplication app;
// 	app.weights = weights;
// 	app.numLayers = ctx.numLayers;
// 	app.layerSize = ctx.layerSize;
// 	try {
// 		app.run();
// 	} catch (const std::runtime_error& e) {
// 		printf("%s\n", e.what());
		
// 	}
// }

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