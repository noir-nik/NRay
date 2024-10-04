#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

std::shared_ptr<spdlog::logger> Logger::_logger;

void Logger::Init(){
	std::vector<spdlog::sink_ptr> sinks;
	// stdout sink
	sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks[0]->set_pattern("%^[%T.%e] [%^%L%$] %v");
	// file sink
	// sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true));
	// sinks[1]->set_pattern("[%T] [%l] %v");

	_logger = std::make_shared<spdlog::logger>("Double", sinks.begin(), sinks.end());
	spdlog::register_logger(_logger);
	_logger->set_level(spdlog::level::trace);
	_logger->flush_on(spdlog::level::trace);
	spdlog::set_default_logger(_logger);
}

std::shared_ptr<spdlog::logger>& Logger::Get(){
	return _logger;
}
