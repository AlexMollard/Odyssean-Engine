#pragma once
#include <boost/filesystem.hpp>
#include <boost/stacktrace.hpp>
#include <iostream>
#include <signal.h>
#include <source_location>
#include <thread>

#if defined(_MSC_VER) && (!__clang__)
// BreakPoint on line and output in console just in case using the printf format
#define S_ASSERT(inValue, message)                                                                                                                                              \
	{                                                                                                                                                                           \
		if (!(inValue))                                                                                                                                                         \
		{                                                                                                                                                                       \
			const std::source_location location = std::source_location::current();                                                                                              \
			std::clog << "ASSERT:\n"                                                                                                                                            \
					  << "file: " << location.file_name() << "(" << location.line() << ":" << location.column() << ") " << location.function_name() << ": " << message << '\n'; \
			__debugbreak();                                                                                                                                                     \
		}                                                                                                                                                                       \
	}

// Same as above just don't breakpoint
#define S_WARN(inValue, message)                                                                                                                                                \
	{                                                                                                                                                                           \
		if (!(inValue))                                                                                                                                                         \
		{                                                                                                                                                                       \
			const std::source_location location = std::source_location::current();                                                                                              \
			std::clog << "WARN:\n"                                                                                                                                              \
					  << "file: " << location.file_name() << "(" << location.line() << ":" << location.column() << ") " << location.function_name() << ": " << message << '\n'; \
		}                                                                                                                                                                       \
	}

// Just a info output with file info
#define S_INFO(message)                                                                                                                                                     \
	{                                                                                                                                                                       \
		const std::source_location location = std::source_location::current();                                                                                              \
		std::clog << "Info:\n"                                                                                                                                              \
				  << "file: " << location.file_name() << "(" << location.line() << ":" << location.column() << ") " << location.function_name() << ": " << message << '\n'; \
	}
#endif

// Warning but not with file info
#define S_WARN_NO_FILE(message) std::clog << "\nWARN: " << message << '\n';

#if defined(__clang__)
// BreakPoint on line and output in console just in case
#define S_ASSERT(inValue, message)

// Same as above just don't breakpoint
#define S_WARN(inValue, message)

// Just a info output with file info
#define S_INFO(message)

#endif

// Wrap the boost stacktrace in a namespace so we dont get redifinition errors
namespace StackTrace
{
// Get the stack trace
inline std::string GetStackTrace(const std::vector<boost::stacktrace::frame>& frames)
{
	std::ifstream ifs("backtrace.dump");

	std::string callStack;
	for (auto& frame : frames) { callStack += frame.source_file() + "(" + std::to_string(frame.source_line()) + "): " + frame.name() + "\n"; }

	return callStack;
}

// Just a info output with file info
inline void ThreadStack(const std::string& message)
{
	std::vector<boost::stacktrace::frame> frames = boost::stacktrace::basic_stacktrace().as_vector();
	frames.erase(frames.begin(), frames.begin() + 3);

	std::cout << "PRINTING CALLSTACK" << std::endl;
	// thread GetStackTrace as it can take a while to get the stack trace
	std::thread stackTraceThread([frames]() {
		std::string callStack = GetStackTrace(frames);
		std::clog << "Call Stack:\n" << callStack << '\n';
	});
	stackTraceThread.detach();
}
} // namespace StackTrace

#define S_TRACE(message) StackTrace::ThreadStack(message);