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
	void MainLoop();
	void Finish();

	NeuralSdfInfo* info;
	std::vector<float> weights;
};