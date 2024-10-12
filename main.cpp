#include "Pch.hpp"
// #include "VulkanBase.hpp"

// #include "NeuralSdf.hpp"

#include "VulkanBase.hpp"

#include "FileManager.hpp"

#include "tests/Test.hpp"

#include <filesystem>

// Example
int main(int argc, char* argv[])
{
	Logger::Init();
	// Test::Test(Test::NeuralSdf);
	// Test::Test(Test::Slang);
	Test::Test(Test::Feature);
	return 0;
}