#pragma once

#include <string>
#include <cstdint>
#include <vector>

struct NeuralSdfInfo
{
	std::string weightsPath;
	int numLayers; // Number of hidden layers
	int layerSize; // Size of a hidden layer
	std::string outputPath;
	int width;
	int height;
};


class NeuralSdfApplication {
public:
	void run(NeuralSdfInfo* pNeuralSdfInfo);
private:
	void Setup();
	void Create();
	void Compute();
	void Finish();

	NeuralSdfInfo* info;
	std::vector<float> weights;
};

// Example
// int main(int argc, char* argv[])
// {
// 	Logger::Init();
// 	NeuralSdfApplication app;
// 	NeuralSdfInfo neuralSdfInfo = {
// 		.weightsPath = "assets/sdf1_weights.bin",
// 		.numLayers = 2,
// 		.layerSize = 64,
// 		.outputPath = "output.bmp",
// 		.width = 512,
// 		.height = 512,
// 	};
// 	app.run(&neuralSdfInfo);
// 	return 0;
// }