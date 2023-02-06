#pragma once
#include <csignal>
#include <exception>
#include <iostream>
#include "filter.hpp"
#include "stacktrace.hpp"
#include "util.hpp"

#if defined(_POSIX_C_SOURCE) and _POSIX_C_SOURCE >= 200809L
#include <cstring>
#define STACKTRACE_HAS_STRSIGNAL
#endif

namespace zh {

void terminate_hook() {
	std::exception_ptr eptr = std::current_exception();
	if (eptr) {
		try {
			std::rethrow_exception(eptr);
		} catch (const std::exception& e) {
#ifdef STACKTRACE_HAS_RTTI
			std::cerr << "Unhandled " << detail::typename_string(e) << ": " << e.what() << "\n";
#else
			std::cerr << "Unhandled exception: " << e.what() << "\n";
#endif
		} catch (...) {
			std::cerr << "Unknown unhandled exception\n";
		}
	} else {
		std::cerr << "Terminate called without an exception\n";
	}

	auto trace = stacktrace();
	drop_frames_until_throw(trace);
	drop_frames_after_main(trace);

	std::cerr << "Stacktrace:\n";
	std::cerr << std::setw(2) << trace;

	std::exit(1);
}

void signal_hook(int signal) {
	std::cerr << "Received signal " << signal << ": ";
#ifdef STACKTRACE_HAS_STRSIGNAL
	const char* signal_name = sigabbrev_np(signal);
	if (signal_name) {
		std::cerr << "SIG" << signal_name;
	} else {
		std::cerr << "unknown signal";
	}
#else
	switch (signal) {
		case SIGTERM:
			std::cerr << "SIGTERM";
			break;
		case SIGSEGV:
			std::cerr << "SIGSEGV";
			break;
		case SIGINT:
			std::cerr << "SIGINT";
			break;
		case SIGILL:
			std::cerr << "SIGILL";
			break;
		case SIGABRT:
			std::cerr << "SIGABRT";
			break;
		case SIGFPE:
			std::cerr << "SIGFPE";
			break;
		default:
			std::cerr << "unknown signal";
	}
#endif
	std::cerr << "\n";

	auto trace = stacktrace();
	if (signal == SIGABRT) {
		drop_frames_until(trace, "abort");
	}
	drop_frames_after_main(trace);

	std::cerr << "Stacktrace:\n";
	std::cerr << std::setw(2) << trace;

	std::exit(1);
}

}  // namespace zh
