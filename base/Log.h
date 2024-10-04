#pragma once

#define SPDLOG_COMPILED_LIB
// #include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#define LOG_INFO(...) Logger::Get()->info(__VA_ARGS__);
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__);
#define LOG_ERROR(...) spdlog::error("{}", fmt::format(__VA_ARGS__));
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__);
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__); exit(1);

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__);

#ifdef APP_DEBUG
#define DEBUG_TRACE(...) Logger::Get()->trace(__VA_ARGS__)
#else
#define DEBUG_TRACE(...)
#endif

class Logger {
public:
	static void Init();
	static std::shared_ptr<spdlog::logger>& Get();
private:
	static std::shared_ptr<spdlog::logger> _logger;
};
