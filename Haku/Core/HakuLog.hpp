#pragma once
#include "spdlog/spdlog.h"

void		init();
void		info(std::string& ref);
void		warn(std::string& ref);
void		error(std::string& ref);
void		critical(std::string& ref);
std::string stringify(const char* args);
std::string stringify(const std::string args);

template<typename T>
std::string stringify(T args)
{
	return std::to_string(args);
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

#define HAKU_LOG_INIT()		init();
#define HAKU_LOG_INFO(...)	log_info(__FUNCTION__, __VA_ARGS__)
#define HAKU_LOG_WARN(...)	log_warn(__FUNCTION__, __VA_ARGS__)
#define HAKU_LOG_CRIT(...)	log_critical(__FUNCTION__, __VA_ARGS__)
#define HAKU_LOG_ERROR(...) log_error(__FUNCTION__, __VA_ARGS__)
#else
#define HAKU_LOG_INIT()
#define HAKU_LOG_INFO(...)
#define HAKU_LOG_WARN(...)
#define HAKU_LOG_CRIT(...)
#define HAKU_LOG_ERROR(...)
#endif
