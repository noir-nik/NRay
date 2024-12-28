
#ifdef USE_MODULES
import Log;
#else
#include "Log.cppm"
#endif
#include "tests/Test.hpp"


// Example
int main(int argc, char* argv[])
{
	Logger::Init();
	Test::Test(Test::Feature);
	return 0;
}