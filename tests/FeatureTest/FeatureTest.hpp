#if (!defined SLANG) && (!defined GLSL)
#pragma once
#include <string>
#include <cstdint>
#include <vector>

struct FeatureTestInfo
{
	int width;
	int height;
};

class FeatureTestApplication {
public:
	void run(FeatureTestInfo* pFeatureTestInfo);
private:
	void Setup();
	void Create();
	void Draw();
	void Compute();
	void MainLoop();
	void Finish();

	FeatureTestInfo* info;
};
#endif



struct FeatureTestConstants {
	int width;
	int height;
	int storageImageRID;
	int pad[1];
};