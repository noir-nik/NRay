#ifdef USE_MODULES
module;
#endif

#ifdef USE_MODULES
module Log;
import spdlog;
import stl;
#else
#define SPDLOG_COMPILED_LIB
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "Log.cppm"
#endif


std::shared_ptr<spdlog::logger> Logger::_logger;

constexpr bool log_to_file = 0;
constexpr bool use_time_in_log_filename = 1;

void Logger::Init(){
	std::vector<spdlog::sink_ptr> sinks;
	// stdout sink
	sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks[0]->set_pattern("%^[%T.%e] [%^%L%$] %v");
	// file sink
	if (log_to_file) {
		char filename[64];
		if (use_time_in_log_filename) {
			auto now = std::time(nullptr);
			auto tm = *std::localtime(&now);
			std::strftime(filename, sizeof(filename), "logs/log-%F-%H-%M-%S.txt", &tm);
			// std::strftime(filename, sizeof(filename), "logs/log-%F-%T.txt", &tm);
		}
		else {
			strcpy(filename, "logs/log.txt");
		}
		sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true));
		sinks[1]->set_pattern("[%T.%e] [%L] %v");
		// sinks[0]->set_pattern("%^[%T.%e] [%^%L%$] %v");

		// sinks[1]->set_level(spdlog::level::warn);
	}

	_logger = std::make_shared<spdlog::logger>("Double", sinks.begin(), sinks.end());
	spdlog::register_logger(_logger);
	_logger->set_level(spdlog::level::trace);
	_logger->flush_on(spdlog::level::trace);
	spdlog::set_default_logger(_logger);
}

std::shared_ptr<spdlog::logger>& Logger::Get(){
	return _logger;
}
