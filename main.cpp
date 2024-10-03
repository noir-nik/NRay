#include "Pch.h"
#include "core/VulkanBase.h"

int main()
{
	Logger::Init();
    spdlog::info("Welcome to spdlog!");
	LOG_INFO("Welcome to spdlog!");
	vkw::Init();
}