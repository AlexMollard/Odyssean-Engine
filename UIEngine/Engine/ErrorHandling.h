#pragma once
#include <iostream>
#include <source_location>

#if defined(_MSC_VER) && (!__clang__)
// BreakPoint on line and output in console just in case using the printf format
#define S_ASSERT(inValue, message)                                                                                  \
	if (!(inValue))                                                                                                 \
	{                                                                                                               \
		const std::source_location location = std::source_location::current();                                      \
		std::clog << "ASSERT:\n"                                                                                    \
				  << "file: " << location.file_name() << "(" << location.line() << ":" << location.column() << ") " \
				  << location.function_name() << ": " << message << '\n';                                           \
		__debugbreak();                                                                                             \
	}

// Same as above just don't breakpoint
#define S_WARN(inValue, message)                                                                                    \
	if (!(inValue))                                                                                                 \
	{                                                                                                               \
		const std::source_location location = std::source_location::current();                                      \
		std::clog << "WARN:\n"                                                                                      \
				  << "file: " << location.file_name() << "(" << location.line() << ":" << location.column() << ") " \
				  << location.function_name() << ": " << message << '\n';                                           \
	}

// Just a info output with file info
#define S_INFO(message)                                                                                         \
	const std::source_location location = std::source_location::current();                                      \
	std::clog << "Info:\n"                                                                                      \
			  << "file: " << location.file_name() << "(" << location.line() << ":" << location.column() << ") " \
			  << location.function_name() << ": " << message << '\n';
#endif

#if defined(__clang__)
// BreakPoint on line and output in console just in case
#define S_ASSERT(inValue, message)

// Same as above just don't breakpoint
#define S_WARN(inValue, message)

// Just a info output with file info
#define S_INFO(message)

#endif