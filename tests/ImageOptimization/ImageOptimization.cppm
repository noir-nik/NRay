#ifdef ENGINE
#ifdef USE_MODULES
export module ImageOptimization;
#define _IMAGEOPTIMIZATION_EXPORT export
#else
#pragma once
#define _IMAGEOPTIMIZATION_EXPORT

#include <string>
#include <cstdint>
#include <vector>
#endif

_IMAGEOPTIMIZATION_EXPORT
struct ImageOptimizationInfo
{
	int width;
	int height;
	float learningRate;
	int numIterations;
};

_IMAGEOPTIMIZATION_EXPORT
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

_IMAGEOPTIMIZATION_EXPORT
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