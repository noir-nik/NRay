#ifdef ENGINE
#ifdef USE_MODULES
export module NeuralSdf;
import std;
#define _NEURALSDF_EXPORT export
#else
#pragma once
#define _NEURALSDF_EXPORT

#include <string>
#include <vector>
#endif


struct NeuralSdfInfo
{
	std::string weightsPath;
	int numLayers; // Number of hidden layers
	int layerSize; // Size of a hidden layer
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
#endif


struct NeuralSdfConstants {
	int width;
	int height;
	int numLayers;
	int layerSize;

	int weightsRID;
	int outputImageRID;
	int imageGPU;
	int pad[1];
};
