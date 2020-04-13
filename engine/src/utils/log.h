
/*
 *	Configuration dependent logging
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__

#ifdef __LOGGING__

	#include <core/logger.h>

	#define LOG_CHANGE_COLOR_RED()		engine::core::Logger::ChangeColor(engine::core::LoggerColor::RED)
	#define LOG_CHANGE_COLOR_GREEN()	engine::core::Logger::ChangeColor(engine::core::LoggerColor::GREEN)
	#define LOG_CHANGE_COLOR_YELLOW()	engine::core::Logger::ChangeColor(engine::core::LoggerColor::YELLOW)
	#define LOG_CHANGE_COLOR_BLUE()		engine::core::Logger::ChangeColor(engine::core::LoggerColor::BLUE)
	#define LOG_CHANGE_COLOR_MAGENTA()	engine::core::Logger::ChangeColor(engine::core::LoggerColor::MAGENTA)
	#define LOG_CHANGE_COLOR_CYAN()		engine::core::Logger::ChangeColor(engine::core::LoggerColor::CYAN)
	#define LOG_CHANGE_COLOR_WHITE()	engine::core::Logger::ChangeColor(engine::core::LoggerColor::WHITE)
	#define LOG_COLOR_RESET()			engine::core::Logger::ChangeColor(engine::core::LoggerColor::ORIGINAL)

	#define LOG(...) engine::core::Logger::Log(__VA_ARGS__)
	
	#define LOG_TRACE(...)	LOG_CHANGE_COLOR_CYAN();	LOG("[LOG] "); LOG(__VA_ARGS__);  LOG_COLOR_RESET(); LOG("\n")
	#define LOG_INFO(...)	LOG_CHANGE_COLOR_GREEN();	LOG("[LOG] "); LOG(__VA_ARGS__);  LOG_COLOR_RESET(); LOG("\n")
	#define LOG_WARN(...)	LOG_CHANGE_COLOR_YELLOW();	LOG("[LOG] "); LOG(__VA_ARGS__);  LOG_COLOR_RESET(); LOG("\n")
	#define LOG_ERROR(...)	LOG_CHANGE_COLOR_RED();		LOG("[LOG] "); LOG(__VA_ARGS__);  LOG_COLOR_RESET(); LOG("\n")
	#define LOG_FATAL(...)	LOG_CHANGE_COLOR_MAGENTA(); LOG("[LOG] "); LOG(__VA_ARGS__);  LOG_COLOR_RESET(); LOG("\n")
#else
	#define LOG_TRACE(F, ...)
	#define LOG_INFO(F, ...)
	#define LOG_WARN(F, ...)
	#define LOG_ERROR(F, ...)
	#define LOG_FATAL(F, ...)
#endif

#endif