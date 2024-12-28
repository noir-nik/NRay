#ifdef USE_MODULES
module;
#else
#pragma once
#endif

#define SPDLOG_COMPILED_LIB
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

// NOLINTBEGIN(misc-unused-using-decls)
#ifdef USE_MODULES
export module spdlog;

namespace spdlog {
	export using spdlog::logger;
	export using spdlog::sink_ptr;
	export using spdlog::register_logger;
	export using spdlog::set_default_logger;

	namespace sinks {
		export using spdlog::sinks::stdout_color_sink_mt;
		export using spdlog::sinks::basic_file_sink_mt;
	} // namespace sinks

	namespace details {
		export using spdlog::details::registry;
	} // namespace details

	namespace level {
		export using spdlog::level::level_enum;
	} // namespace level
} // namespace spdlog

#endif
// NOLINTEND(misc-unused-using-decls)