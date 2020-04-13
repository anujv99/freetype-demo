
/*
 * Cross platform assert
 */

#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <cassert>
#include <csignal>

#include "log.h"

#if defined(_MSC_VER)
	#define DEBUG_BREAK() __debugbreak()
#endif

#ifdef __LOGGING__
	#define ASSERT_LOG(...) engine::core::Logger::ChangeColor(engine::core::LoggerColor::RED);\
							engine::core::Logger::Log(__VA_ARGS__);\
							engine::core::Logger::ChangeColor(engine::core::LoggerColor::ORIGINAL)
#else
	#define ASSERT_LOG(...)
#endif

#if defined(__ASSERT__) && defined(DEBUG_BREAK)
	#define ASSERTM(X, M) if (!(X)) { ASSERT_LOG("Assertion failed \"%s\", file : %s, line : %d - ", #X, __FILE__, __LINE__); ASSERT_LOG(M); DEBUG_BREAK(); }
	#define ASSERT(X) if (!(X)) { ASSERT_LOG("Assertion failed \"%s\", file : %s, line : %d - ", #X, __FILE__, __LINE__); DEBUG_BREAK(); }
#elif defined(__ASSERT__)
	#define ASSERTM(X, M) if(!(X)) { ASSERT_LOG(M); } assert(X)
	#define ASSERT(X) assert(X)
#else
	#define ASSERTM(X, M)
	#define ASSERT(X)
#endif

#endif //__ASSERT_H__