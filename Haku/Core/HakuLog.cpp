#include "HakuLog.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

static std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("Haku");

void init()
{
	logger->set_pattern("[%n]%^%l:%o %v%$");
}
void info(std::string& ref)
{
	logger->info(ref);
}

void warn(std::string& ref)
{
	logger->warn(ref);
}

void critical(std::string& ref)
{
	logger->critical(ref);
}

void error(std::string& ref)
{
	logger->error(ref);
}


std::string stringify(const char* args)
{
	return std::string(args);
}

std::string stringify(const std::string args)
{
	return args;
}
