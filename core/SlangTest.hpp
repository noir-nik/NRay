#pragma once

#include <string>
#include <cstdint>
#include <vector>

struct SlangTestInfo
{
	std::string weightsPath;
	int numLayers; // Number of hidden layers
	int layerSize; // Size of a hidden layer
	std::string outputPath;
	int width;
	int height;
};


class SlangTestApplication {
public:
	void run(SlangTestInfo* pSlangTestInfo);
private:
	void Setup();
	void Create();
	void Compute();
	void MainLoop();
	void Finish();

	SlangTestInfo* info;
	std::vector<float> weights;
};