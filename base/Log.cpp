#include "Log.h"
std::shared_ptr<spdlog::logger> Logger::_logger;


namespace Log
{
}
void Logger::Init(){
	_logger =std::make_shared<spdlog::logger>("Logger");
	_logger->set_level(spdlog::level::trace);
	_logger->flush_on(spdlog::level::trace);
	spdlog::set_default_logger(_logger);
}
