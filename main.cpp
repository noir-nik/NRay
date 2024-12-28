
#ifdef USE_MODULES
import Log;
import FeatureTest;
#else
#include "Log.cppm"
#include "tests/FeatureTest/FeatureTest.cppm"
#endif
// #include "tests/Test.hpp"

int main(int argc, char* argv[])
{
	Logger::Init();
	FeatureTestApplication app;
	if (argc != 2) {
		LOG_INFO("Please provide a path to gltf file");
	} else {
		app.run(argv[1]);
	}
	// Test::Test(Test::Feature);
	return 0;
}