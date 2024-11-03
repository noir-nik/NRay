#if (!defined SLANG) && (!defined GLSL)
#pragma once
#include <string>
#include <cstdint>
#include <vector>

struct ImageOptimizationInfo
{
	int width;
	int height;
	float learningRate;
	int numIterations;
};

class ImageOptimizationApplication {
public:
	void run(ImageOptimizationInfo* pImageOptimizationInfo);
private:
	void Setup();
	void Create();
	void Compute();
	void CmdOptimizationStep();
	void Finish();

	ImageOptimizationInfo* info;
};
#endif



struct ImageOptConstants {
	int width;
	int height;
	int imageOptRID;
	int imageGTRID;

	int gradRID;
	float learningRate;
	int numIterations;
	int pad[1];
};