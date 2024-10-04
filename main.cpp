#include "Pch.h"
#include "core/VulkanBase.h"

int main()
{
	Logger::Init();
    spdlog::info("Welcome to spdlog!");
	LOG_INFO("Log info");
	LOG_WARN("Log warn");
	LOG_ERROR("Log error");
	LOG_TRACE("Log trace");
	vkw::Init();
}