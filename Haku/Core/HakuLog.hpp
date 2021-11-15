#pragma once
#include "spdlog/spdlog.h"

void info(std::string& ref)
{
	spdlog::info(ref);
}

void warn(std::string& ref)
{
	spdlog::warn(ref);
}

void critical(std::string& ref)
{
	spdlog::critical(ref);
}

void error(std::string& ref)
{
	spdlog::error(ref);
}

template<typename T>
std::string stringify(T args)
{
	return std::to_string(args);
}

std::string stringify(const char* args)
{
	return std::string(args);
}

template<typename T>
void append(std::string& ref, T& param)
{
	ref.append(stringify(param));
}

template<typename T, typename... Targs>
void append(std::string& ref, T param, Targs&... args)
{
	ref.append(stringify(param) + " ");
	append(ref, args...);
}

template<typename T, typename... args>
void log_info(T& param, args&... arg)
{
	std::string ref;
	append(ref, param, arg...);
	info(ref);
}

template<typename T, typename... args>
void log_warn(T& param, args&... arg)
{
	std::string ref;
	append(ref, param, arg...);
	warn(ref);
}

template<typename T, typename... args>
void log_critical(T& param, args&... arg)
{
	std::string ref;
	append(ref, param, arg...);
	critical(ref);
}

template<typename T, typename... args>
void log_error(T& param, args&... arg)
{
	std::string ref;
	append(ref, param, arg...);
	error(ref);
}

#ifdef _DEBUG
#define HAKU_LOG_INIT()
#define HAKU_LOG_INFO(...)	log_info(__VA_ARGS__)
#define HAKU_LOG_WARN(...)	log_warn(__VA_ARGS__)
#define HAKU_LOG_CRIT(...)	log_critical(__VA_ARGS__)
#define HAKU_LOG_ERROR(...) log_error(__VA_ARGS__)
#else
#define HAKU_LOG_INFO(...)
#define HAKU_LOG_WARN(...)
#define HAKU_LOG_CRIT(...)
#define HAKU_LOG_ERROR(...)
#endif
