module;
// #define SPDLOG_COMPILED_LIB
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

// NOLINTBEGIN(misc-unused-using-decls)
export module spdlog;

export
namespace spdlog {
	using spdlog::logger;
	using spdlog::sink_ptr;
	using spdlog::register_logger;
	using spdlog::set_default_logger;

	using spdlog::format_string_t;


	namespace sinks {
		using sinks::stdout_color_sink_mt;
		using sinks::basic_file_sink_mt;
	} // namespace sinks

	namespace details {
		using details::registry;
	} // namespace details

	namespace level {
		using level::level_enum;
	} // namespace level
} // namespace spdlog
// NOLINTEND(misc-unused-using-decls)
