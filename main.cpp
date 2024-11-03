
#include "Log.hpp"
#include "tests/Test.hpp"


// Example
int main(int argc, char* argv[])
{
	Logger::Init();
	Test::Test(Test::Feature);
	return 0;
}