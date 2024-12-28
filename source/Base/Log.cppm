#ifdef USE_MODULES
module;
#define _LOG_EXPORT export
#else
#pragma once
#define _LOG_EXPORT
#endif

#ifdef USE_MODULES
export module Log;
import spdlog;
import stl;
#else
#include <spdlog/spdlog.h>
#endif

_LOG_EXPORT
class Logger {
public:
	static void Init();
	static std::shared_ptr<spdlog::logger>& Get();
	static void Shutdown();
private:
	static std::shared_ptr<spdlog::logger> _logger;
};

#ifdef USE_MODULES
export template <typename... Args>
void LOG_INFO(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	Logger::Get()->info(fmt, std::forward<Args>(args)...);
}

export template <typename... Args>
void LOG_WARN(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	Logger::Get()->warn(fmt, std::forward<Args>(args)...);
}

export template <typename... Args>
void LOG_ERROR(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	Logger::Get()->error(fmt, std::forward<Args>(args)...);
}

export template <typename... Args>
void LOG_DEBUG(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	Logger::Get()->debug(fmt, std::forward<Args>(args)...);
}

export template <typename... Args>
void LOG_CRITICAL(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	Logger::Get()->critical(fmt, std::forward<Args>(args)...);
}

export template <typename... Args>
void LOG_TRACE(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	Logger::Get()->trace(fmt, std::forward<Args>(args)...);
}

#ifdef NRAY_DEBUG
export template <typename... Args>
void DEBUG_TRACE(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	Logger::Get()->trace(fmt, std::forward<Args>(args)...);
}
export template <typename... Args>
void DEBUG_ASSERT(bool condition, spdlog::format_string_t<Args...> fmt, Args &&...args) {
	if (!(condition)) { Logger::Get()->error(fmt, std::forward<Args>(args)...); exit(1); }
}

#else
export template <typename... Args>
void DEBUG_TRACE(...) {}
export template <typename... Args>
void DEBUG_ASSERT(...) {}
#endif

export template <typename... Args>
void ASSERT(bool condition, spdlog::format_string_t<Args...> fmt, Args &&...args) {
	if (!(condition)) { Logger::Get()->error(fmt, std::forward<Args>(args)...); exit(1); }
}
#else


#define LOG_INFO(...) Logger::Get()->info(__VA_ARGS__);
#define LOG_WARN(...) Logger::Get()->warn(__VA_ARGS__);
#define LOG_ERROR(...) Logger::Get()->error(__VA_ARGS__);
#define LOG_DEBUG(...) Logger::Get()->debug(__VA_ARGS__);
#define LOG_CRITICAL(...) Logger::Get()->critical(__VA_ARGS__);

#define LOG_TRACE(...) Logger::Get()->trace(__VA_ARGS__);

#ifdef NRAY_DEBUG
#define DEBUG_ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] {0}", __VA_ARGS__); exit(1); } }
#define DEBUG_TRACE(...) Logger::Get()->trace(__VA_ARGS__)
#else
#define DEBUG_TRACE(...)
#define DEBUG_ASSERT(condition, ...)
#endif

#define ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] in {}:{} {}", __FILE__, __LINE__, __VA_ARGS__); exit(1); } }

#endif
