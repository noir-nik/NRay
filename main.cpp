#include "Pch.hpp"
#include "VulkanBase.hpp"

#include "ImageOptimizer.hpp"

class ComputeApplication {
public:
	uint32_t width = 1024;
	uint32_t height = 1024;

    void run() {
        Setup();
        Create();
        Compute();
        Finish();
    }

private:
	void Setup() {
	}

	void Create() {
		vkw::Init();
		ImageOptimizer::CreateImages(width, height);
		ImageOptimizer::CreateShaders();
	}

	void Compute() {
	}

	void Finish() {
		ImageOptimizer::Destroy();
		vkw::Destroy();
	}
};

int main()
{
	Logger::Init();
	ComputeApplication app;
	app.run();
	return 0;
}