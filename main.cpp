#include "Pch.hpp"
#include "VulkanBase.hpp"


class computeApplication {
public:
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
	}

	void Compute() {
	}

	void Finish() {
		vkw::Destroy();
	}
};

int main()
{
	Logger::Init();
	Application app;
	app.run();
	return 0;
}