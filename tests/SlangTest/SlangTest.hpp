#pragma once

#include <string>
#include <cstdint>
#include <vector>

struct SlangTestInfo
{
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
};